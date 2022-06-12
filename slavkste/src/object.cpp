#include "object.hpp"


using namespace std;

CObject::CObject( TVector<2> position )
        : m_position( position ){}

void CObject::addTag( ETag tag )
{
  m_tag |= tag;
}
