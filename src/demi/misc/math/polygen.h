/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#ifndef Dipolygen_h__
#define Dipolygen_h__


namespace Demi
{
    class DI_MISC_API DiPolygon
    {

    public:
        typedef DiVector<DiVec3>              VertexList;

        typedef std::multimap<DiVec3, DiVec3> EdgeMap;

        typedef DiPair< DiVec3, DiVec3>       Edge;

    public:
        
        DiPolygon();
        
        ~DiPolygon();

        DiPolygon( const DiPolygon& cpy );

    public:

        void             InsertVertex(const DiVec3& vdata, size_t vertexIndex);

        void             InsertVertex(const DiVec3& vdata);

        const DiVec3&    GetVertex(size_t vertex) const;

        void             SetVertex(const DiVec3& vdata, size_t vertexIndex);

        void             RemoveDuplicates(void);

        size_t           GetVertexCount(void) const;

        const DiVec3&    GetNormal(void) const;

        void             DeleteVertex(size_t vertex);

        bool             IsPointInside(const DiVec3& point) const;

        void             StoreEdges(EdgeMap *edgeMap) const;

        void             Reset(void);

        bool             operator == (const DiPolygon& rhs) const;

        bool             operator != (const DiPolygon& rhs) const
                            { return !( *this == rhs ); }


    protected:

        void             UpdateNormal(void) const;
                         
        VertexList       mVertexList;
                         
        mutable DiVec3   mNormal;
                         
        mutable bool     mIsNormalSet;
    };
}

#endif
