#ifndef STORKSIXVECTOR_H
#define STORKSIXVECTOR_H

template <class T>
class StorkSixVector
{
    public:
        StorkSixVector()
        {
            for(G4int i=0;i<6;i++)
            {
                data[i]=0.;
            }
        }
        ~StorkSixVector()
        {

        }
        StorkSixVector(T one,T two,T three,T four,T five,T six)
        {
            data[0]=one;
            data[1]=two;
            data[2]=three;
            data[3]=four;
            data[4]=five;
            data[5]=six;
        }
        StorkSixVector(T* dataP)
        {
            for(G4int i=0;i<6;i++)
            {
                data[i]=dataP[i];
            }
        }
        T operator [] (G4int a)
        {
            return data[a];
        }
        void operator = (T* dataP)
        {
            for(G4int i=0;i<6;i++)
            {
                data[i]=dataP[i];
            }
        }
        void operator = (StorkSixVector dataP)
        {
            for(G4int i=0;i<6;i++)
            {
                data[i]=dataP[i];
            }
        }
        T data[6];
    protected:
    private:

};

#endif // STORKSIXVECTOR_H
