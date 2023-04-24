#pragma once
#include "types.hpp"

namespace BackgroundThread
{
	class Token;
	class BaseTask
	{
		public:
			BaseTask(std::shared_ptr<Token> token) : m_token{token} {};
			virtual void Run(t_forward_task forward) = 0;
			virtual void AbortImmediately() = 0;
			Token * get_Token() { return m_token.get(); };
		
			bool running;
		private:
			
			std::shared_ptr<Token> m_token;
	};
}