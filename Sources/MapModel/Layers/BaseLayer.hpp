#pragma once

#include <vector>

#include "Common.hpp"
#include "Geometry.hpp"

namespace MapControl
{
	const bool defLayerVisibility = true;

	class BaseLayer
	{
	public:
		BaseLayer();

		bool GetVisibility() const noexcept;
		void SetVisibility(bool visibility) noexcept;

	protected:
		bool _visibility;
	};
}