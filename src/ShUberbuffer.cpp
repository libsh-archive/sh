#include <cassert>
#include "ShUberbuffer.hpp"
#include "ShDebug.hpp"
#include "ShBackend.hpp"
#include "ShEnvironment.hpp"

using namespace std;

namespace SH {

ShUberbuffer::ShUberbuffer(int width, int height, int depth,
    int elements, unsigned int format)
  : ShMemoryObject(width, height, depth, elements), 
    m_format(format), m_mem(0) {
}


ShUberbuffer::~ShUberbuffer() {
  if( m_mem != 0 && ShEnvironment::backend ) {
    ShEnvironment::backend->deleteUberbuffer( this );
  }
}

unsigned int ShUberbuffer::format() const {
  return m_format;
}

unsigned int ShUberbuffer::mem() const {
  return m_mem;
}

void ShUberbuffer::setMem( unsigned int mem ) {
  m_mem = mem;
}

void ShUberbuffer::setFormat( unsigned int format ) {
  m_format = format;
}

bool ShUberbuffer::copy( ShUberbufferPtr b ) {
  printf( "copy() %d to %d\n", b->mem(), m_mem );
  if( !compatibleWith( b ) ) return false;
  m_properties = b->properties();
  m_format = b->format();
//  glDeleteMemATI( m_mem );
 // m_mem = glCloneMemATI( b->mem() );

  if( m_mem == 0 ) {
    SH_DEBUG_ERROR( "Unable to clone uber buffer" );
    ////printErr();
  }
  return true;
}

void ShUberbuffer::setData(const float* data) {
  if( ShEnvironment::backend ) {
    ShEnvironment::backend->setUberbufferData( this, data );
  }
}

float* ShUberbuffer::data() const {
  if( ShEnvironment::backend ) {
    return ShEnvironment::backend->getUberbufferData( this );
  }
  return 0;
}

void ShUberbuffer::invalidate() {
  printf( "invalidate() not implemented %d\n", m_mem );
//  glInvalidateMemATI( m_mem );
  //printErr();
}


void ShUberbuffer::setProperty( unsigned int property, int value ) {
  m_properties[property] = value;
}

void ShUberbuffer::unsetProperty( unsigned int property ) {
  m_properties.erase( property );
}

const ShUberbuffer::PropertyMap& ShUberbuffer::properties() {
  return m_properties;
}

void ShUberbuffer::detach() {
  printf( "detach() not implemented %d\n", m_mem );
//  glDetachMemATI( m_mem );
  //printErr();
}


/*
int* ShUberbuffer::getPropertiesArray() {
  int* result = new int[ m_properties.size() * 2 ];
  int i = 0;
  for( PropertyMap::iterator pmit = m_properties.begin();
      pmit != m_properties.end(); ++pmit, i += 2 ) {
    result[i] = pmit->first;
    result[i + 1] = pmit->second;
  }
  return result;
}
*/

} // namespace Sh

