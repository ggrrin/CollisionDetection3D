#include "CornerTable.hpp"

namespace CollisionDetection3D
{

	VertexTriangleIterator::value_type VertexTriangleIterator::operator*(){
		return Triangle(ct.getVertex(triangle), ct.getVertex(ct.cNext(triangle)), ct.getVertex(ct.cPrev(triangle)), ct.getmodelMatrix());
	}

	VertexTriangleIterator::value_type VertexTriangleIterator::operator->(){
		return Triangle(ct.getVertex(triangle), ct.getVertex(ct.cNext(triangle)), ct.getVertex(ct.cPrev(triangle)), ct.getmodelMatrix());
	}

	VertexTriangleIterator::self_type VertexTriangleIterator::operator++(){
		triangle = CornerTable::cPrev(ct.cOpposite(CornerTable::cPrev(triangle)));
		if (triangle < 0 || triangle > ct.getIndexBuffer().size())
			triangle = initialTriangle;
		return (*this);
	}

	VertexTriangleIterator::self_type VertexTriangleIterator::operator++(int){
		triangle = CornerTable::cPrev(ct.cOpposite(CornerTable::cPrev(triangle)));
		if (triangle < 0 || triangle > ct.getIndexBuffer().size())
			triangle = initialTriangle;
		return (*this);
	}

	bool VertexTriangleIterator::operator==(const self_type& it) const{
		if (&it.ct != &ct)
			throw std::logic_error("Iterators iterate over different data!");
		return it.triangle == triangle;
	}

	bool VertexTriangleIterator::operator !=(const self_type& it) const{
		return !(it == *this);
	}
	//////////////////////////////////////////////////////////////////

	PlaneIterator::value_type PlaneIterator::operator*(){//TODO co s model transform ?  aaaaa:D meeeen i almost forgot
		return ct->getPlane(triangle);
	}

	PlaneIterator::value_type PlaneIterator::operator->(){
		return **this;
	}

	PlaneIterator::self_type PlaneIterator::operator++(){
		triangle += 3;
		return (*this);
	}

	PlaneIterator::self_type PlaneIterator::operator++(int){
		triangle += 3;
		return (*this);
	}

	bool PlaneIterator::operator==(const self_type& it) const{
		if (it.ct != ct)
			throw std::logic_error("Iterators iterate over different data!");
		return it.triangle == triangle;
	}

	bool PlaneIterator::operator !=(const self_type& it) const{
		return !(it == *this);
	}

	//////////////////////////////////////////////////////////////////

	TriangleIterator::value_type TriangleIterator::operator*() const{
		return Triangle(ct->vertexBuffer[ct->indexBuffer[triangle]], ct->vertexBuffer[ct->indexBuffer[triangle + 1]], ct->vertexBuffer[ct->indexBuffer[triangle + 2]], ct->getmodelMatrix());
	}

	TriangleIterator::value_type TriangleIterator::operator->() const{
		return **this; //Triangle(ct.vertexBuffer[ct.indexBuffer[triangle]], ct.vertexBuffer[ct.indexBuffer[triangle + 1]], ct.vertexBuffer[ct.indexBuffer[triangle + 2]], ct.getmodelMatrix());
	}

	TriangleIterator::value_type TriangleIterator::operator[](ptrdiff_t offset) const
	{
		auto t = TriangleIterator(*this);
		t.triangle += offset * 3;
		return *t;
	}

	TriangleIterator::self_type TriangleIterator::operator++(){
		triangle += 3;
		return (*this);
	}

	TriangleIterator::self_type TriangleIterator::operator++(int){
		triangle += 3;
		return (*this);
	}

	bool TriangleIterator::operator==(const self_type& it) const{
		if (it.ct != ct)
			throw std::logic_error("Iterators iterate over different data!");
		return it.triangle == triangle;
	}

	bool TriangleIterator::operator !=(const self_type& it) const{
		return !(it == *this);
	}

	vector<TriangleIterator> TriangleIterator::getTriangleNeigbours()
	{
		return move(vector<TriangleIterator>({
			TriangleIterator(*ct, CornerTable::cTriangle(ct->cLeft(triangle))),
			TriangleIterator(*ct, CornerTable::cTriangle(ct->cRight(triangle))),
			TriangleIterator(*ct, CornerTable::cTriangle(ct->cOpposite(triangle)))
		}));
	}



	/////////////////////////////////////////////////////////////

	PlaneIterator PlaneContainer::begin() 
	{
		return PlaneIterator(*this, 0);
	}

	PlaneIterator PlaneContainer::end() 
	{
		return PlaneIterator(*this, ct->indexBuffer.size());
	}

	PlaneContainer::value_type PlaneContainer::getPlane(int triangleIndex)
	{
		auto v1 = ct->vertexBuffer[ct->indexBuffer[triangleIndex]];
		auto v2 = ct->vertexBuffer[ct->indexBuffer[triangleIndex + 1]];
		auto v3 = ct->vertexBuffer[ct->indexBuffer[triangleIndex + 2]];

		if (modelTransformed)//TODO caching
			return OrientatedPlane(trans(ct->getmodelMatrix(), v1), trans(ct->getmodelMatrix(), v2), trans(ct->getmodelMatrix(), v3));
		else
			return OrientatedPlane(v1, v2, v3);
	}

	/////////////////////////////////////////////////////////////

	size_t TriangleContainer::size() const
	{
		return ct->indexBuffer.size() / 3;
	}

	bool TriangleContainer::empty() const
	{
		return size() < 1;
	}
	
	Triangle TriangleContainer::operator[](size_t index) const
	{
		return *TriangleIterator(*ct, index * 3);
	}

	TriangleIterator TriangleContainer::begin() const
	{
		return TriangleIterator(*ct, 0);
	}

	TriangleIterator TriangleContainer::end() const
	{
		return TriangleIterator(*ct, ct->indexBuffer.size());
	}

}