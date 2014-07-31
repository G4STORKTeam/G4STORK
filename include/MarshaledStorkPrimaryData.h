// This file was generated automatically by marshalgen.

#ifndef MarshaledStorkPrimaryData_H
#define MarshaledStorkPrimaryData_H


#include "StorkPrimaryData.hh"
//MSH_include_begin
#include "MarshaledStorkNeutronData.h"
#include "MarshaledStorkMatPropChange.h"
//MSH_include_end

#include <stdio.h>
#include <string.h>
#include "MarshaledObj.h"

  class MarshaledStorkPrimaryData;

  class ShadowedMarshaledStorkPrimaryData : public StorkPrimaryData{
    friend class MarshaledStorkPrimaryData;
};

  class MarshaledStorkPrimaryData : public MarshaledObj {
public:
    StorkPrimaryData* param;
    ShadowedMarshaledStorkPrimaryData* Shadowed_param;
public:


// Function implementations

MarshaledStorkPrimaryData(StorkPrimaryData* objptr) : MarshaledObj() {
    msh_isUnmarshalDone = false;
    this->param = objptr;
    this->Shadowed_param = (ShadowedMarshaledStorkPrimaryData*)this->param;
    if (objptr == NULL)
        return;

    marshal1();
    marshal2();
    marshal3();
    marshal4();
}

MarshaledStorkPrimaryData(void *buf, char isUnmarshaling = 'u')
: MarshaledObj(buf, isUnmarshaling) {
    msh_isUnmarshalDone = false;
}

~MarshaledStorkPrimaryData() {
    //if(msh_isUnmarshalDone && this->param != NULL) {
        //delete this->param;
    //}
}

StorkPrimaryData* unmarshal() {
    //We don't want to unmarshal the buffer is empty.
    if(msh_size <= MSH_HEADER_SIZE) {
        //This is buggy, we can't always assume that
        //obj == NULL <==> List is empty.
        return NULL;
    } else {
        {
	param = new StorkPrimaryData();
	}
        this->Shadowed_param = (ShadowedMarshaledStorkPrimaryData*)this->param;
        this->msh_isUnmarshalDone = true;
        unmarshal1();
        unmarshal2();
        unmarshal3();
        unmarshal4();
        return this->param;
    }
}

void unmarshalTo(StorkPrimaryData* obj) {
    //We don't want to unmarshal the buffer is empty.
    if(msh_size <= MSH_HEADER_SIZE) {
        //This is buggy, we can't always assume that
        //obj == NULL <==> List is empty.
        return;
    } else {
        this->param = obj;
        this->Shadowed_param = (ShadowedMarshaledStorkPrimaryData*)this->param;
        this->msh_isUnmarshalDone = true;
        unmarshal1();
        unmarshal2();
        unmarshal3();
        unmarshal4();
    }
}

void marshal1() {
    //declare field_size to be the size of this field
    int msh_currentSize = 0;
    if (isUnmarshaling())
        throw "Tried to marshal in obj marked isUnmarshaling == true";

    //Copy the sizespec into msh_currentSize here:
    {

    }

    //Increase the size of buffer if needed
    EXTEND_BUFFER(msh_currentSize + sizeof(int) + sizeof(int)); // 4 bytes for the total size of field, 4 bytes for the number of elements in the array (in the case of array marshaling)
    //Mark the beginning position for this field, will write the total size of this field here later
    msh_field_begin = msh_cursor;

    //Advance cursor of distance = sizeof(int)
    msh_cursor += sizeof(int);

    //Now just copy "get" functions here
    {
	int copy_off = 0;
	int elementNum;
	 elementNum = param->primaries->size(); 
	memcpy( msh_cursor+copy_off, &elementNum,sizeof(int));
	copy_off += sizeof(int);
	for(int index=0;index<elementNum;index++){
			StorkNeutronData anElement;
			 anElement = (*(param->primaries))[index]; 
			MarshaledStorkNeutronData marEle(&anElement);
			EXTEND_BUFFER(marEle.getBufferSize());
			memcpy(msh_cursor+copy_off, marEle.getBuffer(), marEle.getBufferSize());
			copy_off += marEle.getBufferSize();
		}
	msh_currentSize = copy_off;

    }
    //Now advance the cursor
    msh_cursor += msh_currentSize;
    //Now set the size of this field
    int tmp; //use memcpy instead of *(int*)... =... to prevent bus error
    tmp = (msh_cursor-msh_field_begin) - sizeof(int);
    memcpy(msh_field_begin, &tmp, sizeof(int));

    //Now set msh_size
    msh_size = msh_cursor - msh_buffer;
    MSH_SET_TOTALSIZE(msh_size);    MSH_SET_TYPECHOICE(msh_typechoice);
}

void unmarshal1() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
		int copy_off = 0;
		int elementNum;
		memcpy(&elementNum, msh_cursor+copy_off, sizeof(int));
		copy_off += sizeof(int);
		for(int index=0;index<elementNum;index++){
			MarshaledStorkNeutronData marEle(msh_cursor+copy_off);
			StorkNeutronData *anElement = (StorkNeutronData *)marEle.unmarshal();
			copy_off += marEle.getBufferSize();
			 param->primaries->push_back(*anElement); 
		}

    }
    msh_cursor += msh_currentSize;
}

void marshal2() {
    //declare field_size to be the size of this field
    int msh_currentSize = 0;
    if (isUnmarshaling())
        throw "Tried to marshal in obj marked isUnmarshaling == true";

    //Copy the sizespec into msh_currentSize here:
    {
	msh_currentSize = sizeof(G4long);

    }

    //Increase the size of buffer if needed
    EXTEND_BUFFER(msh_currentSize + sizeof(int) + sizeof(int)); // 4 bytes for the total size of field, 4 bytes for the number of elements in the array (in the case of array marshaling)
    //Mark the beginning position for this field, will write the total size of this field here later
    msh_field_begin = msh_cursor;

    //Advance cursor of distance = sizeof(int)
    msh_cursor += sizeof(int);

    //Now just copy "get" functions here
    {
	memcpy(msh_cursor, &Shadowed_param->eventSeed, sizeof(G4long));
    }
    //Now advance the cursor
    msh_cursor += msh_currentSize;
    //Now set the size of this field
    int tmp; //use memcpy instead of *(int*)... =... to prevent bus error
    tmp = (msh_cursor-msh_field_begin) - sizeof(int);
    memcpy(msh_field_begin, &tmp, sizeof(int));

    //Now set msh_size
    msh_size = msh_cursor - msh_buffer;
    MSH_SET_TOTALSIZE(msh_size);    MSH_SET_TYPECHOICE(msh_typechoice);
}

void unmarshal2() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
	memcpy(&Shadowed_param->eventSeed, msh_cursor, sizeof(G4long));

    }
    msh_cursor += msh_currentSize;
}

void marshal3() {
    //declare field_size to be the size of this field
    int msh_currentSize = 0;
    if (isUnmarshaling())
        throw "Tried to marshal in obj marked isUnmarshaling == true";

    //Copy the sizespec into msh_currentSize here:
    {
	msh_currentSize = sizeof(G4int);

    }

    //Increase the size of buffer if needed
    EXTEND_BUFFER(msh_currentSize + sizeof(int) + sizeof(int)); // 4 bytes for the total size of field, 4 bytes for the number of elements in the array (in the case of array marshaling)
    //Mark the beginning position for this field, will write the total size of this field here later
    msh_field_begin = msh_cursor;

    //Advance cursor of distance = sizeof(int)
    msh_cursor += sizeof(int);

    //Now just copy "get" functions here
    {
	memcpy(msh_cursor, &Shadowed_param->eventNum, sizeof(G4int));
    }
    //Now advance the cursor
    msh_cursor += msh_currentSize;
    //Now set the size of this field
    int tmp; //use memcpy instead of *(int*)... =... to prevent bus error
    tmp = (msh_cursor-msh_field_begin) - sizeof(int);
    memcpy(msh_field_begin, &tmp, sizeof(int));

    //Now set msh_size
    msh_size = msh_cursor - msh_buffer;
    MSH_SET_TOTALSIZE(msh_size);    MSH_SET_TYPECHOICE(msh_typechoice);
}

void unmarshal3() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
	memcpy(&Shadowed_param->eventNum, msh_cursor, sizeof(G4int));

    }
    msh_cursor += msh_currentSize;
}

void marshal4() {
    //declare field_size to be the size of this field
    int msh_currentSize = 0;
    if (isUnmarshaling())
        throw "Tried to marshal in obj marked isUnmarshaling == true";

    //Copy the sizespec into msh_currentSize here:
    {

    }

    //Increase the size of buffer if needed
    EXTEND_BUFFER(msh_currentSize + sizeof(int) + sizeof(int)); // 4 bytes for the total size of field, 4 bytes for the number of elements in the array (in the case of array marshaling)
    //Mark the beginning position for this field, will write the total size of this field here later
    msh_field_begin = msh_cursor;

    //Advance cursor of distance = sizeof(int)
    msh_cursor += sizeof(int);

    //Now just copy "get" functions here
    {
	int copy_off = 0;
	int elementNum;
	 elementNum = param->propChanges->size(); 
	memcpy( msh_cursor+copy_off, &elementNum,sizeof(int));
	copy_off += sizeof(int);
	for(int index=0;index<elementNum;index++){
			StorkMatPropChange anElement;
			 anElement = (*(param->propChanges))[index]; 
			MarshaledStorkMatPropChange marEle(&anElement);
			EXTEND_BUFFER(marEle.getBufferSize());
			memcpy(msh_cursor+copy_off, marEle.getBuffer(), marEle.getBufferSize());
			copy_off += marEle.getBufferSize();
		}
	msh_currentSize = copy_off;

    }
    //Now advance the cursor
    msh_cursor += msh_currentSize;
    //Now set the size of this field
    int tmp; //use memcpy instead of *(int*)... =... to prevent bus error
    tmp = (msh_cursor-msh_field_begin) - sizeof(int);
    memcpy(msh_field_begin, &tmp, sizeof(int));

    //Now set msh_size
    msh_size = msh_cursor - msh_buffer;
    MSH_SET_TOTALSIZE(msh_size);    MSH_SET_TYPECHOICE(msh_typechoice);
}

void unmarshal4() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
		int copy_off = 0;
		int elementNum;
		memcpy(&elementNum, msh_cursor+copy_off, sizeof(int));
		copy_off += sizeof(int);
		for(int index=0;index<elementNum;index++){
			MarshaledStorkMatPropChange marEle(msh_cursor+copy_off);
			StorkMatPropChange *anElement = (StorkMatPropChange *)marEle.unmarshal();
			copy_off += marEle.getBufferSize();
			 param->propChanges->push_back(*anElement); 
		}

    }
    msh_cursor += msh_currentSize;
}

};
#endif

