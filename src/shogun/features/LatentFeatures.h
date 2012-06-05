/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2012 Viktor Gal
 * Copyright (C) 2012 Viktor Gal
 */

#ifndef __LATENTFEATURES_H__
#define __LATENTFEATURES_H__

#include <shogun/features/Features.h>
#include <shogun/features/LatentLabels.h>

namespace shogun
{
  class CLatentFeatures : public CFeatures
  {
    public:
      /** default constructor */
      CLatentFeatures ();

      CLatentFeatures (int32_t num_samples);

      virtual ~CLatentFeatures ();

      virtual CFeatures* duplicate () const;

      /** get feature type
       *
       * @return templated feature type
       */
      virtual EFeatureType get_feature_type ();

      /** get feature class
       *
       * @return feature class
       */
      virtual EFeatureClass get_feature_class ();

      /** get number of examples
       *
       * @return number of examples/vectors (possibly of subset, if implemented)
       */
      virtual int32_t get_num_vectors () const;

      /** get memory footprint of one feature
       *
       * abstract base method
       *
       * @return memory footprint of one feature
       */
      virtual int32_t get_size ();

      virtual const char* get_name () const { return "LatentFeatures"; }

      bool add_sample (CLatentData* example);
      CLatentData* get_sample (index_t idx);

    protected:
      CDynamicObjectArray<CLatentData>* m_samples;

    private:
      void init ();
  };
}

#endif /* __LATENTFEATURES_H__ */
