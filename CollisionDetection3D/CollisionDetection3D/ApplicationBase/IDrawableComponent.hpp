#pragma once
#include "IDrawable.hpp"
#include "IAppComponent.hpp"

namespace CollisionDetection3D
{
	using namespace std;

	class ApplicationCore;

	class IDrawableComponent : public IAppComponent, public IDrawable
	{
	public:
		IDrawableComponent(ApplicationCore* app) : IAppComponent(app), IDrawable(app) {}
		virtual ~IDrawableComponent(){}

	protected:
		virtual void onDraw() {};
		virtual void onInitialize() {};

		void initialize() override final
		{
			onInitialize();
			IDrawable::initialize();
		}

		void draw() override final
		{
			onDraw();
			IDrawable::draw();
		}
	};

}