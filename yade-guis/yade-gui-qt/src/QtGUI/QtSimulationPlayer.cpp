/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "QtSimulationPlayer.hpp"
#include "FileDialog.hpp"
#include "GLSimulationPlayerViewer.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/FileGenerator.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtSimulationPlayer::QtSimulationPlayer() : QtGeneratedSimulationPlayer()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtSimulationPlayer::~QtSimulationPlayer()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::pbInputConfigFileClicked()
{
	string selectedFilter;
	string fileName = FileDialog::getOpenFileName("../data", "XML Yade File (*.xml)", "Choose a file to load", this->parentWidget()->parentWidget(),selectedFilter );
 
	if (!fileName.empty() && selectedFilter == "XML Yade File (*.xml)")
		leInputConfigFile->setText(fileName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void QtSimulationPlayer::pbInputDirectoryClicked()
{
	string directory = FileDialog::getExistingDirectory ( "../data","Choose the directory where the recorded file are", this->parentWidget()->parentWidget());
	if (!directory.empty())
		leInputDirectory->setText(directory);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::pbLoadClicked()
{
	glSimulationPlayerViewer->load(leInputConfigFile->text());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::pbOutputDirectoryClicked()
{
	string directory = FileDialog::getExistingDirectory ( "../data","Choose the directory where to save the sna shots", this->parentWidget()->parentWidget());
	if (!directory.empty())
		leOutputDirectory->setText(directory.c_str());

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::pbPlayClicked()
{	
	setParameters();

	glSimulationPlayerViewer->startAnimation();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::pbPauseClicked()
{	
	glSimulationPlayerViewer->stopAnimation();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::pbStepClicked()
{
	glSimulationPlayerViewer->stopAnimation();
	glSimulationPlayerViewer->doOneStep();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::pbResetClicked()
{
	setParameters();
	glSimulationPlayerViewer->reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::cbSaveSnapShotsToggled(bool b)
{
	glSimulationPlayerViewer->setSaveSnapShots(b);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::closeEvent(QCloseEvent *e)
{
	QtGeneratedSimulationPlayer::closeEvent(e);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSimulationPlayer::setParameters()
{
	glSimulationPlayerViewer->setInputPaddle(sbInputPaddle->value());
	glSimulationPlayerViewer->setInputBaseName(leInputBaseName->text());
	glSimulationPlayerViewer->setInputDirectory(leInputDirectory->text());
	
	glSimulationPlayerViewer->setOutputBaseName(leOutputBaseName->text());
	glSimulationPlayerViewer->setOutputDirectory(leOutputDirectory->text());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
