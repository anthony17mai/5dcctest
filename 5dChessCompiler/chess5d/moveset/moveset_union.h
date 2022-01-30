#pragma once

#include "moveset\moveset.h"

namespace game
{
	namespace moveset
	{
		class move_union : public ab_move_set
		{
		public:
			using entry = std::unique_ptr<ab_move_set>;

			static std::unique_ptr<move_union> unite(std::unique_ptr<ab_move_set> set1, std::unique_ptr<ab_move_set> set2)
			{
				if (typeid(*set1) == typeid(move_union))
				{
					auto u1 = (move_union*)set1.get();
					if (typeid(*set2) == typeid(move_union))
					{
						auto u2 = (move_union*)set2.get();
						
						for (entry& entries : u2->entries)
						{

						}
					}
					else
					{

					}
				}
				else
				{
					if (typeid(set2) == typeid(move_union))
					{

					}
					else
					{

					}
				}
			}

			std::vector<entry> entries;
		};
	}
}