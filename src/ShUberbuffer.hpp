// -*- C++ -*-
#ifndef SHUBERBUFFER_HPP 
#define SHUBERBUFFER_HPP 

#include <string>
#include <map>
#include "ShRefCount.hpp"
#include "ShMemoryObject.hpp"

namespace SH {

/// Uber buffer memory object, based on ATI's UBER_BUFFER class
class ShUberbuffer;
typedef ShRefCount<ShUberbuffer> ShUberbufferPtr;

class ShUberbuffer: public ShMemoryObject {
  public:
    ShUberbuffer(int width, int height, int depth, int elements, unsigned int format = 0);
    virtual ~ShUberbuffer();

    /* ShUberbuffer methods */
    unsigned int format() const;
    unsigned int mem() const;
    void setMem( unsigned int mem );
    void setFormat( unsigned int format );

    /// special case for copying another uber buffer 
    // Checks that dimensions still match and uses clone method 
    bool copy( ShUberbufferPtr b ); 

    /** \brief Writes data into this memory object
     * Writes a chunk of memory with size width * height * depth * elements
     * into this memory object.
     *
     * WARNING - this function may not work properly.  Use at your own risk
     * until ATI drivers support uploading data into an uber buffer.
     *
     * (Currently this commandeers a texture and uses glTexImage to upload
     * the data, so always do this BEFORE binding the shader to prevent
     * clobbering an active texture)
     */
    void setData(const float *data);

    float* data() const;

 
    /// Invalidates buffer 
    // This indicates to the driver that the next operation on the
    // memory object is to write over all of its data.
    void invalidate();

    typedef std::map< unsigned int, int > PropertyMap; 
    /// sets given property to the provided integer value
    // requires another allocate() to take effect
    void setProperty( unsigned int property, int value );

    /// unsets given property 
    // requires another allocate() to take effect
    void unsetProperty( unsigned int property );

    const PropertyMap& properties();

    void detach();

  protected:
    PropertyMap m_properties;
    unsigned int m_format;
    unsigned int m_mem;

     
    /// outputs error message
    //TODO get rid of this?
    //void printErr() const;

    // returns properties in an array
    // (this should be deleted after use)
 //   int *getPropertiesArray();
};

}

#endif /* SHUBERBUFFER_HPP */
