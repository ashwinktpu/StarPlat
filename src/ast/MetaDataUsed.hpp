#ifndef METADATAUSED_HPP
#define METADATAUSED_HPP

#include <iostream>

struct MetaDataUsed
{
  public:
  bool isMetaUsed;      // if d_meta is used in the ASTNode
  bool isRevMetaUsed;   // if d_rev_meta is used in the ASTNode
  bool isDataUsed;      // if d_data is used in the ASTNode
  bool isSrcUsed;       // if d_src is used in the ASTNode
  bool isWeightUsed;    // if d_weight is used in the ASTNode

  MetaDataUsed() 
  {
    isMetaUsed = false;
    isRevMetaUsed = false;
    isDataUsed = false;
    isSrcUsed = false;
    isWeightUsed = false;
  }

  operator bool() const
  {
    return isMetaUsed || isRevMetaUsed || isDataUsed || isSrcUsed || isWeightUsed;
  }

  MetaDataUsed& operator|= (const MetaDataUsed& rhs)
  {
    isMetaUsed = isMetaUsed || rhs.isMetaUsed;
    isRevMetaUsed = isRevMetaUsed || rhs.isRevMetaUsed;
    isDataUsed = isDataUsed || rhs.isDataUsed;
    isSrcUsed = isSrcUsed || rhs.isSrcUsed;
    isWeightUsed = isWeightUsed || rhs.isWeightUsed;
    return *this;
  }

  MetaDataUsed& operator&= (const MetaDataUsed& rhs)
  {
    isMetaUsed = isMetaUsed && rhs.isMetaUsed;
    isRevMetaUsed = isRevMetaUsed && rhs.isRevMetaUsed;
    isDataUsed = isDataUsed && rhs.isDataUsed;
    isSrcUsed = isSrcUsed && rhs.isSrcUsed;
    isWeightUsed = isWeightUsed && rhs.isWeightUsed;
    return *this;
  }

  MetaDataUsed& operator^= (const MetaDataUsed& rhs)
  {
    isMetaUsed = isMetaUsed ^ rhs.isMetaUsed;
    isRevMetaUsed = isRevMetaUsed ^ rhs.isRevMetaUsed;
    isDataUsed = isDataUsed ^ rhs.isDataUsed;
    isSrcUsed = isSrcUsed ^ rhs.isSrcUsed;
    isWeightUsed = isWeightUsed ^ rhs.isWeightUsed;
    return *this;
  }

  MetaDataUsed& operator= (const MetaDataUsed& rhs)
  {
    isMetaUsed = rhs.isMetaUsed;
    isRevMetaUsed = rhs.isRevMetaUsed;
    isDataUsed = rhs.isDataUsed;
    isSrcUsed = rhs.isSrcUsed;
    isWeightUsed = rhs.isWeightUsed;
    return *this;
  }

  MetaDataUsed operator| (const MetaDataUsed& rhs) const
  {
    MetaDataUsed result = *this;
    result |= rhs;
    return result;
  }

  MetaDataUsed operator& (const MetaDataUsed& rhs) const
  {
    MetaDataUsed result = *this;
    result &= rhs;
    return result;
  }

  MetaDataUsed operator^ (const MetaDataUsed& rhs) const
  {
    MetaDataUsed result = *this;
    result ^= rhs;
    return result;
  }

  MetaDataUsed operator~ () const
  {
    MetaDataUsed result = *this;
    result.isMetaUsed = !result.isMetaUsed;
    result.isRevMetaUsed = !result.isRevMetaUsed;
    result.isDataUsed = !result.isDataUsed;
    result.isSrcUsed = !result.isSrcUsed;
    result.isWeightUsed = !result.isWeightUsed;
    return result;
  }

  void print()
  {
    std::cout << "isMetaUsed: " << isMetaUsed << std::endl;
    std::cout << "isRevMetaUsed: " << isRevMetaUsed << std::endl;
    std::cout << "isDataUsed: " << isDataUsed << std::endl;
    std::cout << "isSrcUsed: " << isSrcUsed << std::endl;
    std::cout << "isWeightUsed: " << isWeightUsed << std::endl;
  }
};

#endif