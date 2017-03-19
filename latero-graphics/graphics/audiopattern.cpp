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

#include "audiopattern.h"
#include <iostream>
#include "../xml.h"
#include "../mask.h"

namespace latero {
namespace graphics { 

Sound::Sound(const Point &surfaceSize) :
	mask_(Mask::Create(surfaceSize))
{
};

void Sound::SetFile(std::string file)
{
	printf("Sound: setting file to %s\n", file.c_str());
	file_ = file;
	UpdateStream();
}

std::string Sound::GetFile()
{
	return file_;
}

void Sound::UpdateStream()
{
#ifdef ENABLE_AUDIERE
	if ((dev_) && (file_!=""))
	{
		printf("Sound: opening stream %s... ", file_.c_str());
		stream_ = audiere::OpenSound(dev_, file_.c_str(), false);
		printf("%s\n", (stream_)?"success":"failure");
	}
#endif
}

void Sound::Play(AudioDevicePtr dev)
{
#ifdef ENABLE_AUDIERE
	if (!stream_)
	{
		printf("Sound: stream does not exist\n");
		dev_ = dev;
		UpdateStream();
	}

	if (stream_)
	{
		if (!stream_->isPlaying())
		{
			printf("Sound: playing stream\n");
			stream_->play();
		}
	}
#endif
}





AudioPattern::AudioPattern(const latero::Tactograph *dev) :
	Pattern(dev),
	prevSound_(-1)
{
}

AudioPattern::AudioPattern(const latero::Tactograph *dev, const XMLInputNode &rootNode) :
	Pattern(dev),
	prevSound_(-1)
{
	std::vector<XMLInputNode> soundNodes = rootNode.GetChildren("sound");
	for (unsigned int i=0; i<soundNodes.size(); ++i)
	{
		XMLInputNode soundNode = soundNodes[i];
		XMLInputNode audioNode = soundNode.GetChild("audiofile");

		Sound *sound = new Sound(dev->GetSurfaceSize());
		sound->SetFile(audioNode.GetFilename());
		AddModifiableChild(sound->mask_);
		sound->mask_->LoadXML(soundNode);
		sounds_.push_back(sound);
	}
}	
	
AudioPattern::AudioPattern(const latero::Tactograph *dev, std::string file) :
Pattern(dev),
prevSound_(-1)
{		
	Sound *sound = new Sound(dev->GetSurfaceSize());
	sound->SetFile(file);
	sounds_.push_back(sound);
}
    
void AudioPattern::AddSound(MaskPtr mask, std::string audioFile)
{
    // NOT SAFE, SHOULD BE DONE WITHIN LOCKS
    Sound *sound = new Sound(Dev()->GetSurfaceSize());
    sound->mask_ = mask;
    sound->SetFile(audioFile);
    AddModifiableChild(sound->mask_);
    sounds_.push_back(sound);        
}

AudioPattern::~AudioPattern()
{
	for (unsigned int i=0; i<sounds_.size(); ++i)
	{
		RemoveModifiableChild(sounds_[i]->mask_);
		delete sounds_[i];
	}
}

void AudioPattern::AppendXML(XMLOutputNode &root) const
{
	XMLOutputNode node = root.AddChild("Pattern");
	node.SetType("Audio");

	for (unsigned int i=0; i<sounds_.size(); ++i)
	{
		XMLOutputNode soundnode = node.AddChild("sound");
		soundnode.AddFilenameChild("audiofile", sounds_[i]->GetFile());
		sounds_[i]->mask_->AppendXML(soundnode);
	}
}

void AudioPattern::Render_(const State *state, latero::BiasedImg &frame)
{
	frame.Set(0);
}

void AudioPattern::PlayAudio(AudioDevicePtr dev, Point pos)
{
	for (unsigned int i=0; i<sounds_.size(); ++i)
	{
		// check whether the center of the display is on a non-zero part of the mask
		if (sounds_[i]->mask_->Get(pos) > 0)
		{
			if (prevSound_ != (int)i)
			{
				printf("AudioPattern: playing sound %d\n", i);
				prevSound_ = i;
      				sounds_[i]->Play(dev); // TODO: will not repeat the same sound
			}
			return;
		}
	}
	prevSound_ = -1; // no sound was played
}
    
Cairo::RefPtr<Cairo::Pattern> AudioPattern::GetDrawingPattern(Cairo::RefPtr<Cairo::Context> cr, boost::posix_time::time_duration t)
{
	cr->push_group();
	return cr->pop_group();
}

} // namespace graphics
} // namespace latero

