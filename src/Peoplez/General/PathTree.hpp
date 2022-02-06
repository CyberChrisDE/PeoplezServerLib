/**
 * Copyright 2017, 2019 Christian Geldermann
 *
 * This file is part of PeoplezServerLib.
 *
 * PeoplezServerLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PeoplezServerLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PeoplezServerLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von PeoplezServerLib.
 *
 * PeoplezServerLib ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * PeoplezServerLib wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit
 * PeoplezServerLib erhalten haben. Wenn nicht, siehe
 * <http://www.gnu.org/licenses/>.
 */

#ifndef PEOPLEZ_GENERAL_PATHTREE_HPP_
#define PEOPLEZ_GENERAL_PATHTREE_HPP_

// Local includes
#include "../String/PeoplezString.hpp"

// External includes
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <unordered_map>

namespace Peoplez
{
	namespace General
	{
		template<typename T>
		class PathTree final
		{
		private:
			class Node final
			{
				friend class PathTree<T>;

			public:
				Node() {}
				template<typename I>
				std::shared_ptr<T> const & GetTarget(I & __restrict__ now, I & __restrict__ end)
				{
					if(now == end) return target;
					else
					{
						typename std::unordered_map<String::PeoplezString, std::unique_ptr<Node> >::const_iterator const iter = childs.find(*now);

						if(iter == childs.end()) return NOT_FOUND;
						else
						{
							++now;
							return iter->second->GetTarget(now, end);
						}
					}
				}

				template<typename I>
				void InsertTarget(I & __restrict__ now, I & __restrict__ end, std::shared_ptr<T> & __restrict__ tar)
				{
					if(now == end)
					{
						if(target.get() == 0) target = tar;
						// else throw exception
					}
					else
					{
						std::unique_ptr<Node> & child = childs[*now];

						if(child.get() == 0) child.reset(new Node());

						++now;

						child->InsertTarget(now, end, tar);
					}
				}

				bool IsEmpty() noexcept
				{
					return target.get() == 0 && childs.empty();
				}

				template<typename I>
				void RemoveTarget(I & __restrict__ now, I & __restrict__ end)
				{
					if(now == end) target = NOT_FOUND;
					else
					{
						typename std::unordered_map<String::PeoplezString, std::unique_ptr<Node> >::const_iterator const iter = childs.find(*now);

						if(iter != childs.end())
						{
							iter->second->RemoveTarget(++now, end);

							if(iter->second->IsEmpty()) childs.erase(iter);
						}
					}
				}
			private:
				static std::shared_ptr<T> const NOT_FOUND;

				Node(Node const & other) : target(other.target)
				{
					for(typename std::unordered_map<String::PeoplezString, std::unique_ptr<Node> >::const_iterator iter = childs.begin(); iter != childs.end(); ++iter)
					{
						childs.insert(std::pair<String::PeoplezString, std::unique_ptr<Node> >(iter->first->UniqueCopy(), std::unique_ptr<Node>(new Node(*(iter->second->get())))));
					}
				}
				Node(Node const && other) = delete;
				Node & operator= (Node const &) = delete;
				Node & operator= (Node const &&) = delete;

				std::unordered_map<String::PeoplezString, std::unique_ptr<Node> > childs;
				std::shared_ptr<T> target = NOT_FOUND;
			};
		public:
			PathTree() {}
			/**
			 * Copy constructor
			 */
			PathTree(PathTree const & other) : child(other.child) {}
			/**
			 * Move constructor
			 */
			PathTree(PathTree && other)
			{
				std::swap(child, other.child);
			}

			virtual ~PathTree() {}

			/**
			 * Copy assignment operator
			 *
			 * @param rhs Right hand side of assignment
			 *
			 * @return Reference to *this
			 */
			PathTree & operator=(PathTree const & rhs)
			{
				child = rhs.child;

				return *this;
			}
			/**
			 * Move assignment operator
			 *
			 * @param rhs Right hand side of assignment
			 *
			 * @return Reference to *this
			 */
			PathTree & operator=(PathTree const && rhs)
			{
				std::swap(child, rhs.child);

				return *this;
			}

			/**
			 * Getter for target object of an url
			 *
			 * @param iter Iterator to the first path object (like container.begin())
			 * @param end Iterator to the end of the path (like container.end())
			 *
			 * @return Shared pointer to searched target or Node::NOT_FOUND if no such target exists
			 */
			template<typename I>
			std::shared_ptr<T> GetTarget(I iter, I end)
			{
				std::shared_lock<std::shared_timed_mutex> lock(mut);

				return child.GetTarget(iter, end);
			}
			/**
			 * Adds a new target at a specific path in the path tree
			 *
			 * @param iter Iterator to the first path object (like container.begin())
			 * @param end Iterator to the end of the path (like container.end())
			 * @param target Target to add to the path tree
			 */
			template<typename I>
			void InsertTarget(I iter, I end, std::shared_ptr<T> target)
			{
				std::unique_lock<std::shared_timed_mutex> lock(mut);

				child.InsertTarget(iter, end, target);
			}
			/**
			 * Indicates whether the path tree is empty
			 *
			 * @return True if no path is in the path tree; False otherwise
			 */
			bool IsEmpty() noexcept
			{
				std::shared_lock<std::shared_timed_mutex> lock(mut);

				return child.IsEmpty();
			}
			/**
			 * Removes the target at a specific path in the path tree
			 *
			 * @param iter Iterator to the first path object (like container.begin())
			 * @param end Iterator to the end of the path (like container.end())
			 */
			template<typename I>
			void RemoveTarget(I iter, I end)
			{
				std::unique_lock<std::shared_timed_mutex> lock(mut);

				child.RemoveTarget(iter, end);
			}

		private:
			std::shared_timed_mutex mut;
			Node child;
		};

		template<typename T>
		std::shared_ptr<T> const PathTree<T>::Node::NOT_FOUND = std::shared_ptr<T>((T *) 0);
	} // namespace General
} // namespace Peoplez

#endif // PEOPLEZ_GENERAL_PATHTREE_HPP_
