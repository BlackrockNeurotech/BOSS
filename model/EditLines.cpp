/////////////////////////////////////////////////////////////////
///
/// @file EditLines.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  July 28, 2015
///
/// @brief EditLines contains information for the edit EditLines
///
/// @note
/// Edit EditLines are created in WaveformGraphicsView. They are a way of defining a unit - all waveforms that intersect all the EditLines.
///

#include "../model/EditLines.h"

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for EditLines class - initializes class variables
EditLines::EditLines()
{
    m_data.clear();
}
/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for EditLines class - initializes class variables
EditLines::EditLines(const EditLines &other)
    : QSharedData(other) 
{ 
    m_data.clear();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Destructor for EditLines class
EditLines::~EditLines()
{
    m_data.clear();
}

