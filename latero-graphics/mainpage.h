// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

/** 
 * \mainpage latero-graphics Documentation
 *
 * \section intro_sec Introduction
 *
 * Welcome to the documentation for the latero-graphics library.
 *
 * \section faq_sec Frequently Asked Questions
 *
 * \subsection faq_sec_underscore Why do some methods end with an underscore?
 *
 * While benchmarking the library, it became obvious that the high granulity of the mutexes
 * used by the rendering objects was very costly computationally. Each object had its own mutex,
 * resulting in a large number of locking and unlocking operations in the critical haptic
 * rendering loop. To resolve this issue, all objects now share a single mutex which is
 * locked only once at the beginning of the haptic rendering loop. Operations that do not
 * require high performance, such as UI-related functions, continue to lock the mutex
 * recursively for each operation.
 *
 * The notation foo() vs foo_() is a result of this design choice. A method ending with an underscore
 * generally does not lock the shared mutex. A method ending without an underscore may or may not lock
 * depending on the type of class. Note that on occasion the undercore is used to denote the 
 * implementation used by another method of the same name.
 *
 * \subsection faq_sec_prot_constructors Why are many constructors protected?
 * 
 * Many classes are intended to be used using smart pointers. Declaring these classes' contructors
 * protected prevents them from being instantiated directly by accident and forces the use of the
 * Create(...) methods for instantiation.
 *
 * \subsection faq_sec_const_dev Why is the device often passed as a const pointer?
 * 
 * The latero::Tactograph device is not thread-safe and is intended to be operated only by
 * the TactileEngine object. Most classes in the library neverthless have access to a 
 * const pointer to the device such that they can perform safe operations such as querying
 * the number of actuators of the array.
 */
