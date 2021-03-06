/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2014 Parijat Mazumdar
 */

#include <shogun/clustering/KMeansMiniBatch.h>
#include <shogun/mathematics/Math.h>
#include <shogun/distance/Distance.h>
#include <shogun/features/DenseFeatures.h>

using namespace shogun;

namespace shogun
{
CKMeansMiniBatch::CKMeansMiniBatch():CKMeansBase()
{
	init_mb_params();
}

CKMeansMiniBatch::CKMeansMiniBatch(int32_t k_i, CDistance* d_i, bool use_kmpp_i):CKMeansBase(k_i, d_i, use_kmpp_i)
{
	init_mb_params();
}

CKMeansMiniBatch::CKMeansMiniBatch(int32_t k_i, CDistance* d_i, SGMatrix<float64_t> centers_i):CKMeansBase(k_i, d_i, centers_i)
{
	init_mb_params();
}

CKMeansMiniBatch::~CKMeansMiniBatch()
{
}

void CKMeansMiniBatch::set_batch_size(int32_t b)
{
	REQUIRE(b>0, "Parameter bach size should be > 0");
	batch_size=b;
}

int32_t CKMeansMiniBatch::get_batch_size() const
{
	return batch_size;
}

void CKMeansMiniBatch::set_mb_iter(int32_t i)
{
	REQUIRE(i>0, "Parameter number of iterations should be > 0");
	minib_iter=i;
}

int32_t CKMeansMiniBatch::get_mb_iter() const
{
	return minib_iter;
}

void CKMeansMiniBatch::set_mb_params(int32_t b, int32_t t)
{
	REQUIRE(b>0, "Parameter bach size should be > 0");
	REQUIRE(t>0, "Parameter number of iterations should be > 0");
	batch_size=b;
	minib_iter=t;
}

void CKMeansMiniBatch::minibatch_KMeans()
{
	REQUIRE(batch_size>0,
		"batch size not set to positive value. Current batch size %d \n", batch_size);
	REQUIRE(minib_iter>0,
		"number of iterations not set to positive value. Current iterations %d \n", minib_iter);

	CDenseFeatures<float64_t>* lhs=
		CDenseFeatures<float64_t>::obtain_from_generic(distance->get_lhs());
	CDenseFeatures<float64_t>* rhs_mus=new CDenseFeatures<float64_t>(0);
	CFeatures* rhs_cache=distance->replace_rhs(rhs_mus);
	rhs_mus->set_feature_matrix(mus);
	int32_t XSize=lhs->get_num_vectors();
	int32_t dims=lhs->get_num_features();

	SGVector<float64_t> v=SGVector<float64_t>(k);
	v.zero();

	for (int32_t i=0; i<minib_iter; i++)
	{
		SGVector<int32_t> M=mbchoose_rand(batch_size,XSize);
		SGVector<int32_t> ncent=SGVector<int32_t>(batch_size);
		for (int32_t j=0; j<batch_size; j++)
		{
			SGVector<float64_t> dists=SGVector<float64_t>(k);
			for (int32_t p=0; p<k; p++)
				dists[p]=distance->distance(M[j],p);

			int32_t imin=0;
			float64_t min=dists[0];
			for (int32_t p=1; p<k; p++)
			{
				if (dists[p]<min)
				{
					imin=p;
					min=dists[p];
				}
			}
			ncent[j]=imin;
		}
		for (int32_t j=0; j<batch_size; j++)
		{
			int32_t near=ncent[j];
			SGVector<float64_t> c_alive=rhs_mus->get_feature_vector(near);
			SGVector<float64_t> x=lhs->get_feature_vector(M[j]);
			v[near]+=1.0;
			float64_t eta=1.0/v[near];
			for (int32_t c=0; c<dims; c++)
			{
				c_alive[c]=(1.0-eta)*c_alive[c]+eta*x[c];
			}
		}
	}
	SG_UNREF(lhs);
	distance->replace_rhs(rhs_cache);
	delete rhs_mus;
}

SGVector<int32_t> CKMeansMiniBatch::mbchoose_rand(int32_t b, int32_t num)
{
	SGVector<int32_t> chosen=SGVector<int32_t>(num);
	SGVector<int32_t> ret=SGVector<int32_t>(b);
	chosen.zero();
	int32_t ch=0;
	while (ch<b)
	{
		const int32_t n=CMath::random(0,num-1);
		if (chosen[n]==0)
		{
			chosen[n]+=1;
			ret[ch]=n;
			ch++;
		}
	}
	return ret;
}

void CKMeansMiniBatch::init_mb_params()
{
	batch_size=-1;
	minib_iter=-1;
}

bool CKMeansMiniBatch::train_machine(CFeatures* data)
{
	initialize_training(data);
	minibatch_KMeans();
	compute_cluster_variances();	
	return true;	
}

}
