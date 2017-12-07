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
#include "Signal.hpp"

// External includes
#include <signal.h>

namespace Peoplez
{
	namespace System
	{
		std::list<Signal *> Signal::sigabrtList;
		std::list<Signal *> Signal::sigfpeList;
		std::list<Signal *> Signal::sigillList;
		std::list<Signal *> Signal::sigintList;
		std::list<Signal *> Signal::sigsegvList;
		std::list<Signal *> Signal::sigtermList;

		std::mutex Signal::mut;

		Signal::Signal(std::function<void(void)> const _target, SignalType const type) : target(_target)
		{
			std::unique_lock<std::mutex> lock(mut);

			//TODO Add exceptions
			switch(type)
			{
			case Abort:
				if(sigabrtList.empty()) signal((int) SIGABRT, Signal::fireSigABRT);
				sigabrtList.push_back(this);
				break;
			case FloatingPointException:
				if(sigfpeList.empty()) signal((int) SIGFPE, Signal::fireSigFPE);
				sigfpeList.push_back(this);
				break;
			case IllegalInstruction:
				if(sigillList.empty()) signal((int) SIGILL, Signal::fireSigILL);
				sigillList.push_back(this);
				break;
			case Interrupt:
				if(sigintList.empty()) signal((int) SIGINT, Signal::fireSigINT);
				sigintList.push_back(this);
				break;
			case SegmentationViolation:
				if(sigsegvList.empty()) signal((int) SIGSEGV, Signal::fireSigSEGV);
				sigsegvList.push_back(this);
				break;
			case Terminate:
				if(sigtermList.empty()) signal((int) SIGTERM, Signal::fireSigTERM);
				sigtermList.push_back(this);
				break;
			default:
				break;
			}
		}

		void Signal::fireSigABRT(int const _ign)
		{
			std::unique_lock<std::mutex> const lock(mut);

			for(std::list<Signal *>::const_iterator iter = sigabrtList.begin(); iter != sigabrtList.end(); ++iter)
			{
				(*iter)->target();
			}
		}

		void Signal::fireSigFPE(int const _ign)
		{
			std::unique_lock<std::mutex> const lock(mut);

			for(std::list<Signal *>::const_iterator iter = sigfpeList.begin(); iter != sigfpeList.end(); ++iter)
			{
				(*iter)->target();
			}
		}

		void Signal::fireSigILL(int const _ign)
		{
			std::unique_lock<std::mutex> const lock(mut);

			for(std::list<Signal *>::const_iterator iter = sigillList.begin(); iter != sigillList.end(); ++iter)
			{
				(*iter)->target();
			}
		}

		void Signal::fireSigINT(int const _ign)
		{
			std::unique_lock<std::mutex> const lock(mut);

			for(std::list<Signal *>::const_iterator iter = sigintList.begin(); iter != sigintList.end(); ++iter)
			{
				(*iter)->target();
			}
		}

		void Signal::fireSigSEGV(int const _ign)
		{
			std::unique_lock<std::mutex> const lock(mut);

			for(std::list<Signal *>::const_iterator iter = sigsegvList.begin(); iter != sigsegvList.end(); ++iter)
			{
				(*iter)->target();
			}
		}

		void Signal::fireSigTERM(int const _ign)
		{
			std::unique_lock<std::mutex> const lock(mut);

			for(std::list<Signal *>::const_iterator iter = sigtermList.begin(); iter != sigtermList.end(); ++iter)
			{
				(*iter)->target();
			}
		}

		Signal::~Signal()
		{
			std::unique_lock<std::mutex> lock(mut);

			switch(sigType)
			{
			case Abort:
				sigabrtList.remove(this);
				if(sigabrtList.empty()) signal((int) SIGABRT, SIG_DFL);
				break;
			case FloatingPointException:
				sigfpeList.remove(this);
				if(sigfpeList.empty()) signal((int) SIGFPE, SIG_DFL);
				break;
			case IllegalInstruction:
				sigillList.remove(this);
				if(sigillList.empty()) signal((int) SIGILL, SIG_DFL);
				break;
			case Interrupt:
				sigintList.remove(this);
				if(sigintList.empty()) signal((int) SIGINT, SIG_DFL);
				break;
			case SegmentationViolation:
				sigsegvList.remove(this);
				if(sigsegvList.empty()) signal((int) SIGSEGV, SIG_DFL);
				break;
			case Terminate:
				sigtermList.remove(this);
				if(sigtermList.empty()) signal((int) SIGTERM, SIG_DFL);
				break;
			default:
				break;
			}
		}
	} // namespace System
} // namespace Peoplez
