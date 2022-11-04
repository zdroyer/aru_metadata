/* Copyright (C) 2003 Scott Wheeler <wheeler@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <iomanip>
#include <stdio.h>

#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>
#include <wavfile.h>
#include <wavproperties.h>

using namespace std;


class GUANOFile: public TagLib::RIFF::WAV::File
{
  public:
  GUANOFile(TagLib::FileName file, bool readProperties = true, 
    TagLib::RIFF::WAV::Properties::ReadStyle propertiesStyle = TagLib::RIFF::WAV::Properties::Properties::Average)
      :TagLib::RIFF::WAV::File::File(file,readProperties, propertiesStyle)
    {

    }
  TagLib::ByteVector getGuanoMetadata()
  {
    for(int i = 0; i< this->chunkCount(); i++){
      auto cn = this->chunkName(i);
      if( cn.startsWith("guan") ){
        return this->chunkData(i);
      }
    }
    return TagLib::ByteVector();
  }
  virtual ~GUANOFile(){}

};



int main(int argc, char *argv[])
{
  if(argc<2){
    cerr<<"You must give at least one filename ;-)" << endl;
    exit(1);
  }
  for(int i = 1; i < argc; i++) {
    std::string file(argv[i]);
    cout << "******************** \"" << argv[i] << "\" ********************" << endl;

    TagLib::FileRef f(file.c_str());

    if(!f.isNull() && f.tag()) {

      TagLib::Tag *tag = f.tag();

      cout << "-- TAG (basic) --" << endl;
      cout << "title   - \"" << tag->title()   << "\"" << endl;
      cout << "artist  - \"" << tag->artist()  << "\"" << endl;
      cout << "album   - \"" << tag->album()   << "\"" << endl;
      cout << "year    - \"" << tag->year()    << "\"" << endl;
      cout << "comment - \"" << tag->comment() << "\"" << endl;
      cout << "track   - \"" << tag->track()   << "\"" << endl;
      cout << "genre   - \"" << tag->genre()   << "\"" << endl;

      TagLib::PropertyMap tags = f.file()->properties();

      unsigned int longest = 0;
      for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
        if (i->first.size() > longest) {
          longest = i->first.size();
        }
      }

      cout << "-- TAG (properties) --" << endl;
      for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
        for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j) {
          cout << left << std::setw(longest) << i->first << " - " << '"' << *j << '"' << endl;
        }
      }

    }

    if(!f.isNull()){
        if( f.audioProperties()) {

        TagLib::AudioProperties *properties = f.audioProperties();

        int seconds = properties->length() % 60;
        int minutes = (properties->length() - seconds) / 60;

        cout << "-- AUDIO --" << endl;
        cout << "bitrate     - " << properties->bitrate() << endl;
        cout << "sample rate - " << properties->sampleRate() << endl;
        cout << "channels    - " << properties->channels() << endl;
        cout << "length      - " << minutes << ":" << setfill('0') << setw(2) << seconds << endl;
      }

      GUANOFile gf(file.c_str());
      auto metadata = gf.getGuanoMetadata(); 
      auto n = metadata.size();
      auto ie = metadata.isEmpty(); 
      auto in = metadata.isNull();
      if( ! metadata.isEmpty() ){
        cout << "-- GUANO --" << endl;
        cout << metadata << endl;
      
      }
    }
  }
  return 0;
}
