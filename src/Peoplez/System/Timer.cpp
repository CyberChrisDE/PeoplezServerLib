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

// Own headers
#include "Timer.hpp"

namespace Peoplez
{
	namespace System
	{
		std::mutex Timer::mut_list;
		std::mutex Timer::mut_startstop;
		//std::atomic_bool Timer::running(false);
		Timer * volatile Timer::timeouts;
		std::thread * Timer::timerThread = 0;
		std::condition_variable Timer::condition;

		Timer::Timer(std::function<void ()> const target, std::chrono::seconds const interval) : duration(interval), nextTimeout(std::chrono::system_clock::now() + interval), before(0), next(0), target(target)
		{
			std::unique_lock<std::mutex> const outerLock(mut_startstop);
			{
				std::unique_lock<std::mutex> const lock_list(mut_list);

				Add();
			}

			// If timer thread is not running ...
			if(!timerThread)
			{
				// Start timer thread
				//running = true;
				timerThread = new std::thread(ThreadFunction);
			}
			// Else update waiting time
			else if(timeouts == this) condition.notify_one();
		}

		Timer::~Timer()
		{
			std::unique_lock<std::mutex> const outerLock(mut_startstop);

			{
				std::unique_lock<std::mutex> const lock(mut_list);

				Remove();
			}

			if(timeouts == 0)
			{
				condition.notify_one();
				timerThread->join();
				delete timerThread;
				timerThread = 0;
			}
		}

		void Timer::Add()
		{
			// If this is the first entry ...
			if(!timeouts) timeouts = this;
			// Else if this timeout is the earliest ...
			else if(timeouts->nextTimeout > nextTimeout)
			{
				next = timeouts;
				timeouts = this;
				next->before = this;
			}
			else
			{
				before = timeouts;

				// Step through until before is the last element with earlier timeout
				while(before->next != 0 && before->next->nextTimeout <= nextTimeout) before = before->next;

				// Set remaining connections
				next = before->next;
				before->next = this;
				if(next != 0) next->before = this;
			}
		}

		void Timer::Remove()
		{
			if(before != 0)
			{
				before->next = next;
				if(next != 0) next->before = before;
			}
			else
			{
				timeouts = next;
				if(next != 0) next->before = 0;
			}

			// Reset before and next
			next = before = 0;
		}

		void Timer::ThreadFunction()
		{
			while(true)
			{
				std::unique_lock<std::mutex> lock(mut_list);

				if(!timeouts) return;

				condition.wait_until(lock, timeouts->nextTimeout);

				if(!timeouts) return;

				if(timeouts->nextTimeout <= std::chrono::system_clock::now())
				{
					timeouts->InnerCB();

					timeouts->nextTimeout += timeouts->duration;
				}

				if(timeouts->next != 0 && timeouts->next->nextTimeout < timeouts->nextTimeout)
				{
					Timer *t = timeouts;
					t->Remove();
					t->Add();
				}
			}
		}
	} // namespace System
} // namespace Peoplez
