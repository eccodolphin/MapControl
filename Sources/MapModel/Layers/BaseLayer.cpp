#include "BaseLayer.hpp"

namespace MapControl
{
	BaseLayer::BaseLayer() :
		_visibility(defLayerVisibility)
	{
	}



	bool BaseLayer::GetVisibility() const noexcept
	{
		return _visibility;
	}

	void BaseLayer::SetVisibility(bool visibility) noexcept
	{
		_visibility = visibility;
	}
}