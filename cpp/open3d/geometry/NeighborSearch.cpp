
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4267)
#endif

#include "open3d/geometry/NeighborSearch.h"
#include <faiss/impl/AuxIndexStructures.h>

namespace open3d {
namespace geometry {

NeighborSearch::NeighborSearch() {}

NeighborSearch::NeighborSearch(const core::Tensor &tensor) { SetTensorData(tensor); }

bool NeighborSearch::SetTensorData(const core::Tensor &tensor){
    core::SizeVector size = tensor.GetShape();
    dimension_ = size[1];
    dataset_size_ = size[0];

    if (dimension_ == 0 || dataset_size_ == 0) {
        utility::LogWarning("[NeighborSearch::SetTensorData] Failed due to no data.");
        return false;
    }

    search_object_.reset(new geometry::KnnFaiss());
    // the following setups are based on 'https://github.com/facebookresearch/faiss/wiki/Guidelines-to-choose-an-index'
    if (dataset_size_ <= 2e5){
        search_object_->SetTensorData(tensor);
    }
    else if (dataset_size_ <= 2e6){
        search_object_->SetTensorData2(tensor, "IVF4096", true, 4096*30);
    }
    else if (dataset_size_ <= 1e7){
        search_object_->SetTensorData2(tensor, "IVF65536_HNSW32", false, 65536*30);
    }
    else if (dataset_size_ <= 1e8){
        search_object_->SetTensorData2(tensor, "IVF262144_HNSW32", false, 262144*30);
    }
    else{
        search_object_->SetTensorData2(tensor, "IVF1048576_HNSW32", false, 1048576*30);
    }
    return true;
}

std::pair<core::Tensor, core::Tensor> 
NeighborSearch::KNNSearch(const core::Tensor& query_tensor, 
                          int knn){
    return search_object_->SearchKNN_Tensor(query_tensor, knn);
}

std::pair<core::Tensor, core::Tensor> 
NeighborSearch::HybridSearch(const core::Tensor& query_tensor, 
                             float radius,
                             int max_knn){
    return search_object_->SearchHybrid_Tensor(query_tensor, radius, max_knn);
}

}  // namespace geometry
}  // namespace open3d

#ifdef _MSC_VER
#pragma warning(pop)
#endif