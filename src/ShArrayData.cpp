// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////

#include "ShArrayData.hpp"
#include <iostream>

namespace SH {
  
  template<typename T, typename  N>
  int ShArrayData<T, N>::assertvalid()const{
    int ret = 1;

    if(!(data.size()==width.size() && data.size()==data_size.size() && data.size()==data_labels.size()&& data.size()==data_copy_flags.size()))
      ret = 0;

    if(!(index.size()==type.size() && index.size()==index_size.size()&& index.size()==index_labels.size() && index.size()==index_copy_flags.size()))
      ret = 0;

    if(ret==0){
      std::cout<<data.size()<<", "<<width.size()<<", "<<data_size.size()<<", "<<data_labels.size()<<", "<<data_copy_flags.size()<<std::endl;
      std::cout<<index.size()<<", "<<type.size()<<", "<<index_size.size()<<", "<<index_labels.size()<<", "<<index_copy_flags.size()<<std::endl;
    }


    return ret;
  }

  template<typename T, typename  N>
  void ShArrayData<T, N>::copy(const ShArrayData<T, N>& o){

    o.assertvalid();
    
    /// copying data 
    for(unsigned int i=0;i<o.getDataSize();i+=1)
      addData(o.getData(i), o.getFormatType(i), o.getDataSize(i), o.getDataLabel(i), true);
    
    /// copying indices
    for(unsigned int i=0;i<o.getIndexSize();i+=1)
      addIndex(o.getIndex(i), o.getRenderType(i), o.getIndexSize(i), o.getDataLabel(i), true);

  }	

  template<typename T, typename  N>
  void ShArrayData<T, N>::clear(){
    

    // before emptying the vectords, we need to delete the buffers that we allocated
  
    // clearing data buffers
    for(unsigned int i=0;i<getDataSize();i+=1)
      if(data_copy_flags[i])
	delete [] data[i];
      
    /// clearing index buffers
    for(unsigned int i=0;i<getIndexSize();i+=1)
      if(index_copy_flags[i])
	delete [] index[i];
  

    // emptying all STL vectors
    data.clear();
    index.clear();
    type.clear();
    width.clear();
    data_size.clear();
    index_size.clear();
    data_labels.clear();
    index_labels.clear();
    data_copy_flags.clear();
    index_copy_flags.clear();


  }

  template<typename T, typename  N>
  ShArrayData<T, N>::~ShArrayData<T, N>(){	
    clear();
  }	

  template<typename T, typename  N>
  ShArrayData<T, N>::ShArrayData<T, N>(){	
    
  }	

  template<typename T, typename  N>
  ShArrayData<T, N>::ShArrayData<T, N>(const ShArrayData<T, N>& o){
    copy(o);
  }

  template<typename T, typename  N>
  ShArrayData<T, N>& ShArrayData<T, N>::operator=(const ShArrayData<T, N>& o){
    clear();
    copy(o);
	return *this;
  }

  template<typename T, typename  N>
  ShArrayData<T, N>& ShArrayData<T, N>::operator+(const ShArrayData<T, N>&){
    return *this;
  }
  
  template<typename T, typename  N>
  ShArrayData<T, N>& ShArrayData<T, N>::operator+=(const ShArrayData<T, N>&){
    return *this;
  }

  template<typename T, typename  N>
  unsigned int ShArrayData<T, N>::getWidth(FormatType f)const{
    int ret;

    switch(f){
    case SH_RGB: 		ret = 3;break;
    case SH_RGBA: 		ret = 4;break;
    case SH_2D:			ret = 2;break;
    case SH_UNKNOWN_FORMATTYPE: ret = 0;break;
      default:
	ret = 0;
      }
    return ret;
  }

  template<typename T, typename  N>
  unsigned int ShArrayData<T, N>::addData(T* d, FormatType w,unsigned int nrec, std::string label, bool bCopy){
    T* mdata;

    if(bCopy){
      mdata = new T[nrec * getWidth(w)];
      memcpy(mdata, d, nrec * getWidth(w)*sizeof(T));
    } else {
      mdata = d;
    }
    
    /// push back all data
    data.push_back(mdata);
    width.push_back(w);
    data_size.push_back(nrec);
    data_labels.push_back(label);
    data_copy_flags.push_back(bCopy);

    return data.size()-1;
  }

  template<typename T, typename  N>
  unsigned int ShArrayData<T, N>::addIndex(N* d, RenderType rtype, unsigned int nrec, std::string label, bool bCopy){
    N* ndata;
    
    if(bCopy){
      ndata = new N[nrec];
      memcpy(ndata, d, nrec *sizeof(N));
    } else {
      ndata = d;
    }
    
    /// push back all data
    index.push_back(ndata);
    type.push_back(rtype);
    index_size.push_back(nrec);
    index_labels.push_back(label);
    index_copy_flags.push_back(bCopy);

    return index.size()-1;
  }
     
  template<typename T, typename  N>
  T* ShArrayData<T, N>::getData(unsigned int i, bool bCopy)const{
    T* mdata;
    
    if(bCopy){
      mdata = new T[data_size[i] * getWidth(width[i])];
      memcpy(mdata, data[i], data_size[i] * getWidth(width[i]) * sizeof(T));
    } else {
      mdata = data[i];
    }
    return mdata;
  }

  
  template<typename T, typename  N>
  N* ShArrayData<T, N>::getIndex(unsigned int i, bool bCopy)const{

     N* ndata;
    
    if(bCopy){
      ndata = new N[index_size[i]];
      memcpy(ndata, data[i], index_size[i] * sizeof(T));
    } else {
      ndata = index[i];
    }
    return ndata;
  }

  template<typename T, typename  N>
  unsigned int ShArrayData<T, N>::getDataSize(unsigned int i)const{
    return data_size[i];
  }

  template<typename T, typename  N>
  unsigned int ShArrayData<T, N>::getDataSize()const{
    return data.size();
  }

  template<typename T, typename  N>
  unsigned int ShArrayData<T, N>::getIndexSize(unsigned int i)const{
    return index_size[i];
  }

  template<typename T, typename  N>
  unsigned int ShArrayData<T, N>::getIndexSize()const{
    return index.size();
  }

  template<typename T, typename  N>
  std::string ShArrayData<T, N>::getDataLabel(unsigned int i)const{
    return data_labels[i];
  }

  template<typename T, typename  N>
  std::string ShArrayData<T, N>::getIndexLabel(unsigned int i)const{
    return index_labels[i];
  }
  
  template<typename T, typename  N>
  FormatType ShArrayData<T, N>::getFormatType(unsigned int i)const{
    return width[i];
  }
  
  template<typename T, typename  N>
  RenderType ShArrayData<T, N>::getRenderType(unsigned int i)const{
    return type[i];
  }

// explicit instantiation here
template class ShArrayData<double, unsigned short>;

}
