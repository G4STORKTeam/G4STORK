// This file was generated automatically by marshalgen.

#ifndef MarshaledStorkTripleFloat_H
#define MarshaledStorkTripleFloat_H


#include "StorkTripleFloat.hh"


#include <stdio.h>
#include <string.h>
#include "MarshaledObj.h"

  class MarshaledStorkTripleFloat;

  class ShadowedMarshaledStorkTripleFloat : public StorkTripleFloat{
    friend class MarshaledStorkTripleFloat;
};

  class MarshaledStorkTripleFloat : public MarshaledObj {
public:
    StorkTripleFloat* param;
    ShadowedMarshaledStorkTripleFloat* Shadowed_param;
public:


// Function implementations

MarshaledStorkTripleFloat(StorkTripleFloat* objptr) : MarshaledObj() {
    msh_isUnmarshalDone = false;
    this->param = objptr;
    this->Shadowed_param = (ShadowedMarshaledStorkTripleFloat*)this->param;
    if (objptr == NULL)
        return;

    marshal1();
}

MarshaledStorkTripleFloat(void *buf, char isUnmarshalingCheck = 'u')
: MarshaledObj(buf, isUnmarshalingCheck) {
    msh_isUnmarshalDone = false;
}

~MarshaledStorkTripleFloat() {
    //if(msh_isUnmarshalDone && this->param != NULL) {
        //delete this->param;
    //}
}

StorkTripleFloat* unmarshal() {
    //We don't want to unmarshal the buffer is empty.
    if(msh_size <= MSH_HEADER_SIZE) {
        //This is buggy, we can't always assume that
        //obj == NULL <==> List is empty.
        return NULL;
    } else {
        {
	param = new StorkTripleFloat();
	}
        this->Shadowed_param = (ShadowedMarshaledStorkTripleFloat*)this->param;
        this->msh_isUnmarshalDone = true;
        unmarshal1();
        return this->param;
    }
}

void unmarshalTo(StorkTripleFloat* obj) {
    //We don't want to unmarshal the buffer is empty.
    if(msh_size <= MSH_HEADER_SIZE) {
        //This is buggy, we can't always assume that
        //obj == NULL <==> List is empty.
        return;
    } else {
        this->param = obj;
        this->Shadowed_param = (ShadowedMarshaledStorkTripleFloat*)this->param;
        this->msh_isUnmarshalDone = true;
        unmarshal1();
    }
}

void marshal1() {
    //declare field_size to be the size of this field
    int msh_currentSize = 0;
    if (isUnmarshaling())
        throw "Tried to marshal in obj marked isUnmarshaling == true";

    //Copy the sizespec into msh_currentSize here:
    {
	msh_currentSize = sizeof(G4ThreeVector);

    }

    //Increase the size of buffer if needed
    EXTEND_BUFFER(msh_currentSize + sizeof(int) + sizeof(int)); // 4 bytes for the total size of field, 4 bytes for the number of elements in the array (in the case of array marshaling)
    //Mark the beginning position for this field, will write the total size of this field here later
    msh_field_begin = msh_cursor;

    //Advance cursor of distance = sizeof(int)
    msh_cursor += sizeof(int);

    //Now just copy "get" functions here
    {
	memcpy(msh_cursor, &Shadowed_param->data, sizeof(G4ThreeVector));
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
	memcpy(&Shadowed_param->data, msh_cursor, sizeof(G4ThreeVector));

    }
    msh_cursor += msh_currentSize;
}

};
#endif

