// This file was generated automatically by marshalgen.

#ifndef MarshaledStorkEventData_H
#define MarshaledStorkEventData_H


#include "StorkEventData.hh"
//MSH_include_begin
#include "MarshaledStorkNeutronData.h"
#include "MarshaledStorkTripleFloat.h"
//MSH_include_end

#include <stdio.h>
#include <string.h>
#include "MarshaledObj.h"

  class MarshaledStorkEventData;

  class ShadowedMarshaledStorkEventData : public StorkEventData{
    friend class MarshaledStorkEventData;
};

  class MarshaledStorkEventData : public MarshaledObj {
public:
    StorkEventData* param;
    ShadowedMarshaledStorkEventData* Shadowed_param;
public:


// Function implementations

MarshaledStorkEventData(StorkEventData* objptr) : MarshaledObj() {
    msh_isUnmarshalDone = false;
    this->param = objptr;
    this->Shadowed_param = (ShadowedMarshaledStorkEventData*)this->param;
    if (objptr == NULL)
        return;

    marshal1();
    marshal2();
    marshal3();
    marshal4();
    marshal5();
    marshal6();
    marshal7();
    marshal8();
    marshal9();
    marshal10();
}

MarshaledStorkEventData(void *buf, char isUnmarshaling = 'u')
: MarshaledObj(buf, isUnmarshaling) {
    msh_isUnmarshalDone = false;
}

~MarshaledStorkEventData() {
    //if(msh_isUnmarshalDone && this->param != NULL) {
        //delete this->param;
    //}
}

StorkEventData* unmarshal() {
    //We don't want to unmarshal the buffer is empty.
    if(msh_size <= MSH_HEADER_SIZE) {
        //This is buggy, we can't always assume that
        //obj == NULL <==> List is empty.
        return NULL;
    } else {
        {
	param = new StorkEventData();
	}
        this->Shadowed_param = (ShadowedMarshaledStorkEventData*)this->param;
        this->msh_isUnmarshalDone = true;
        unmarshal1();
        unmarshal2();
        unmarshal3();
        unmarshal4();
        unmarshal5();
        unmarshal6();
        unmarshal7();
        unmarshal8();
        unmarshal9();
        unmarshal10();
        return this->param;
    }
}

void unmarshalTo(StorkEventData* obj) {
    //We don't want to unmarshal the buffer is empty.
    if(msh_size <= MSH_HEADER_SIZE) {
        //This is buggy, we can't always assume that
        //obj == NULL <==> List is empty.
        return;
    } else {
        this->param = obj;
        this->Shadowed_param = (ShadowedMarshaledStorkEventData*)this->param;
        this->msh_isUnmarshalDone = true;
        unmarshal1();
        unmarshal2();
        unmarshal3();
        unmarshal4();
        unmarshal5();
        unmarshal6();
        unmarshal7();
        unmarshal8();
        unmarshal9();
        unmarshal10();
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
	 elementNum = param->survivors->size(); 
	memcpy( msh_cursor+copy_off, &elementNum,sizeof(int));
	copy_off += sizeof(int);
	for(int index=0;index<elementNum;index++){
			StorkNeutronData anElement;
			 anElement = param->survivors->at(index); 
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
			 param->survivors->push_back(*anElement); 
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
	 elementNum = param->delayed->size(); 
	memcpy( msh_cursor+copy_off, &elementNum,sizeof(int));
	copy_off += sizeof(int);
	for(int index=0;index<elementNum;index++){
			StorkNeutronData anElement;
			 anElement = param->delayed->at(index); 
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

void unmarshal2() {
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
			 param->delayed->push_back(*anElement); 
		}

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
	 elementNum = param->fSites->size(); 
	memcpy( msh_cursor+copy_off, &elementNum,sizeof(int));
	copy_off += sizeof(int);
	for(int index=0;index<elementNum;index++){
			StorkTripleFloat anElement;
			 anElement = param->fSites->at(index); 
			MarshaledStorkTripleFloat marEle(&anElement);
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

void unmarshal3() {
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
			MarshaledStorkTripleFloat marEle(msh_cursor+copy_off);
			StorkTripleFloat *anElement = (StorkTripleFloat *)marEle.unmarshal();
			copy_off += marEle.getBufferSize();
			 param->fSites->push_back(*anElement); 
		}

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
	 elementNum = param->fnEnergy->size(); 
	memcpy( msh_cursor+copy_off, &elementNum,sizeof(int));
	copy_off += sizeof(int);
	for(int index=0;index<elementNum;index++){
			double anElement;
			 anElement = param->fnEnergy->at(index); 
			EXTEND_BUFFER(sizeof(double));
			memcpy(msh_cursor+copy_off, &anElement, sizeof(double));
			copy_off += sizeof(double);
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
			double anElement;
			memcpy(&anElement,msh_cursor+copy_off, sizeof(double));
			copy_off += sizeof(double);
			 param->fnEnergy->push_back(anElement); 
		}

    }
    msh_cursor += msh_currentSize;
}

void marshal5() {
    //declare field_size to be the size of this field
    int msh_currentSize = 0;
    if (isUnmarshaling())
        throw "Tried to marshal in obj marked isUnmarshaling == true";

    //Copy the sizespec into msh_currentSize here:
    {
	msh_currentSize = sizeof(G4double);

    }

    //Increase the size of buffer if needed
    EXTEND_BUFFER(msh_currentSize + sizeof(int) + sizeof(int)); // 4 bytes for the total size of field, 4 bytes for the number of elements in the array (in the case of array marshaling)
    //Mark the beginning position for this field, will write the total size of this field here later
    msh_field_begin = msh_cursor;

    //Advance cursor of distance = sizeof(int)
    msh_cursor += sizeof(int);

    //Now just copy "get" functions here
    {
	memcpy(msh_cursor, &Shadowed_param->totalLifetime, sizeof(G4double));
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

void unmarshal5() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
	memcpy(&Shadowed_param->totalLifetime, msh_cursor, sizeof(G4double));

    }
    msh_cursor += msh_currentSize;
}

void marshal6() {
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
	memcpy(msh_cursor, &Shadowed_param->numNProd, sizeof(G4int));
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

void unmarshal6() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
	memcpy(&Shadowed_param->numNProd, msh_cursor, sizeof(G4int));

    }
    msh_cursor += msh_currentSize;
}

void marshal7() {
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
	memcpy(msh_cursor, &Shadowed_param->numNLost, sizeof(G4int));
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

void unmarshal7() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
	memcpy(&Shadowed_param->numNLost, msh_cursor, sizeof(G4int));

    }
    msh_cursor += msh_currentSize;
}

void marshal8() {
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
	memcpy(msh_cursor, &Shadowed_param->numDProd, sizeof(G4int));
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

void unmarshal8() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
	memcpy(&Shadowed_param->numDProd, msh_cursor, sizeof(G4int));

    }
    msh_cursor += msh_currentSize;
}

void marshal9() {
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

void unmarshal9() {
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

void marshal10() {
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
        memcpy(msh_cursor, &Shadowed_param->numUserCount, sizeof(G4int));
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
      
void unmarshal10() {
    //declare currentSize to be the size of this field
    int msh_currentSize = 0;
    //copy the size of the current field into currentSize
    memcpy(&msh_currentSize, msh_cursor, sizeof(int));
    msh_cursor += sizeof(int);
    //Now copy the setspec here
    {
        memcpy(&Shadowed_param->numUserCount, msh_cursor, sizeof(G4int));
              
    }
        msh_cursor += msh_currentSize;
}
};
#endif

