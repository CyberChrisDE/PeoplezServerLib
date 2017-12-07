/**
 * Copyright 2017 Christian Geldermann
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

#ifndef PEOPLEZ_SYSTEM_TIMER_HPP_
#define PEOPLEZ_SYSTEM_TIMER_HPP_

// External includes
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <atomic>

namespace Peoplez
{
	namespace System
	{
		class Timer final
		{
		public:
			Timer(std::function<void(void)> target, std::chrono::seconds interval);
			virtual ~Timer();

		private:
			Timer(Timer const & other) = delete;

			void Add();
			void InnerCB() {target();}
			void Remove();
			void StartThread();
			void StopThread();

			void operator=(Timer const & other) = delete;

			std::chrono::seconds const duration;

			std::chrono::system_clock::time_point nextTimeout;

			Timer * before;
			Timer * next;
			std::function<void(void)> const target;

			static std::mutex mut_list;
			static std::mutex mut_startstop;
			static Timer * volatile timeouts;
			static std::thread * timerThread;
			static std::condition_variable condition;

			static void ThreadFunction();
		};
	} // namespace System
} // namespace Peoplez

#endif // PEOPLEZ_SYSTEM_TIMER_HPP_
