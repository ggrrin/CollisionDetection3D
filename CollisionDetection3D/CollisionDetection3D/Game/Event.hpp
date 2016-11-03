#include <deque>

namespace CollisionDetection3D
{

	template <typename TArg0>
	class MyEvent
	{
		typedef void(*FuncPtr)(TArg0);
		typedef std::deque<FuncPtr> FuncPtrSeq;

		FuncPtrSeq ls;
	public:
		MyEvent& operator +=(FuncPtr f)
		{
			ls.push_back(f);
			return *this;
		}

		void operator()(TArg0 x)
		{
			for (typename FuncPtrSeq::iterator it(ls.begin()); it != ls.end(); ++it)
				(*it)(x);
		}
	};
}