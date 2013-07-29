//The MIT License (MIT)
//
//Copyright (c) 2013 Mark Farrell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.


#include "Starfall/Entity.h"
#include "Starfall/Config.h"
#include <iostream>
#include <limits>

using namespace Starfall;

Poco::Mutex  Isolates::Mutex;
map<v8::Isolate*, Poco::UInt32> Isolates::Map;

Poco::UInt32 IDGenerator::id = 0;
Poco::Mutex  IDGenerator::mutex;

Poco::Mutex  Entity::CollectionMutex;
Poco::HashMap<Poco::UInt32, Entity::Ptr> Entity::Map;


v8::Isolate* Isolates::Acquire() {

	v8::Isolate* currentIsolate = v8::Isolate::GetCurrent(); //isolate per thread
	if(currentIsolate == NULL) {
		currentIsolate = v8::Isolate::New();
		currentIsolate->Enter(); //enter the isolate when created
	}

	Isolates::Mutex.lock();
	if(Isolates::Map.find(currentIsolate) != Isolates::Map.end()) {
		Isolates::Map[currentIsolate] = Isolates::Map[currentIsolate] + 1;
	} else {
		Isolates::Map[currentIsolate] = 1;
	}
	Isolates::Mutex.unlock();

	return currentIsolate;
}

Poco::UInt32 Isolates::Release(v8::Isolate* releaseIsolate) {
	Isolates::Mutex.lock();
	if(Isolates::Map.find(releaseIsolate) != Isolates::Map.end()) {
		Isolates::Map[releaseIsolate] = Isolates::Map[releaseIsolate] - 1;
		if(Isolates::Map[releaseIsolate] == 0) {
			releaseIsolate->Exit();
			releaseIsolate->Dispose();
			Isolates::Map.erase(releaseIsolate);
		} else {
			return Isolates::Map[releaseIsolate];
		}
	}
	Isolates::Mutex.unlock();
	return 0;
}

DestroyEntityStruct::DestroyEntityStruct() {
	this->sessionid = 0;
}

CreateEntityStruct::CreateEntityStruct() {
	this->sessionid = 0;
	this->mode = 0;
	this->displayName = "";
	this->appearance = "";
}

TransformStruct::TransformStruct() {
	this->action = 0;
}

TransformEntityStruct::TransformEntityStruct() {
	this->sessionid = 0;
}

Poco::UInt32 IDGenerator::next() {	
	Poco::UInt32 nextID = 0;
	IDGenerator::mutex.lock();
	IDGenerator::id++;
	nextID = IDGenerator::id;
	IDGenerator::mutex.unlock();
	return nextID;
}

Entity::Entity() {

	this->isolate = Isolates::Acquire();

	this->persistentContext = IO::Context();
	this->displayName = "";
	//this->appearance = Config::Appearance();
	this->mode = 0;
	this->sessionid = IDGenerator::next();
}

Entity::~Entity() {
	this->persistentContext.Dispose(); //entity's global context must be deleted manually
	Isolates::Release(this->isolate);
}

void Entity::addToPath(TransformStruct transformStruct) {
	this->mutex.lock();
	this->path.push_back(transformStruct);
	this->mutex.unlock();
}

void Entity::clearPath() {
	this->mutex.lock();
	this->path.clear();
	this->mutex.unlock();
}


CreateEntityStruct Entity::castCreateEntityStruct() {
	CreateEntityStruct createEntityStruct; 
	createEntityStruct.sessionid = this->sessionid;
	createEntityStruct.displayName = this->displayName;
	createEntityStruct.mode = this->mode;
	createEntityStruct.position = this->position;
	createEntityStruct.rotation = this->rotation;
	//createEntityStruct.appearance = this->appearance;
	return createEntityStruct;
}

TransformEntityStruct Entity::castTransformEntityStruct() {
	TransformEntityStruct transformEntityStruct; 
	transformEntityStruct.sessionid = this->sessionid;
	for(vector<TransformStruct>::iterator it = this->path.begin(); it != this->path.end(); it++) {
		transformEntityStruct.path.push_back((*it));
	}
	return transformEntityStruct;
}



Entity::Ptr Entity::Create() { 
	Entity::Ptr pEntity = Entity::Ptr(new Entity());
	Entity::CollectionMutex.lock(); //The map isn't thread safe; so we must claim ownership when accessing it.
	Entity::Map[pEntity->sessionid] = pEntity; 
	Entity::CollectionMutex.unlock(); //Release ownership
	return pEntity;
}

vector<Poco::UInt32> Entity::Keys() {
	vector<Poco::UInt32> keys;
	Entity::CollectionMutex.lock();
	for(Poco::HashMap<Poco::UInt32, Entity::Ptr>::Iterator it = Entity::Map.begin(); it != Entity::Map.end(); it++) {
		keys.push_back(it->first);
	}
	Entity::CollectionMutex.unlock();
	return keys;
}


CreateEntityStruct Entity::FindCreateEntityStruct(Poco::UInt32 sessionid) {
	CreateEntityStruct createEntityStruct;
	Entity::Ptr pEntity;
	Entity::CollectionMutex.lock();
	pEntity = Entity::Map[sessionid]; //Note: unlike std::map, Poco::HashMap throws a Poco::NotFoundException that we can use.
	Entity::CollectionMutex.unlock();
	pEntity->mutex.lock(); //lock access to entity's information.
	createEntityStruct = pEntity->castCreateEntityStruct();
	pEntity->mutex.unlock();
	return createEntityStruct;
}

TransformEntityStruct Entity::FindTransformEntityStruct(Poco::UInt32 sessionid) {
	TransformEntityStruct transformEntityStruct;
	Entity::Ptr pEntity;
	Entity::CollectionMutex.lock();
	pEntity = Entity::Map[sessionid]; //Note: unlike std::map, Poco::HashMap throws a Poco::NotFoundException that we can use.
	Entity::CollectionMutex.unlock();
	pEntity->mutex.lock(); //lock access to entity's information.
	transformEntityStruct = pEntity->castTransformEntityStruct();
	pEntity->mutex.unlock();
	return transformEntityStruct;
}

void Entity::Remove(Poco::UInt32 sessionid) {	
	Entity::CollectionMutex.lock();
	Entity::Map.erase(sessionid); //fast erase with a hash
	Entity::CollectionMutex.unlock();
}