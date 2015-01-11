/* GSTEngine.h */

/* Copyright (C) 2012  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define STATE_STOP 0
#define STATE_PLAY 1
#define STATE_PAUSE 2

#define EQ_TYPE_NONE -1
#define EQ_TYPE_KEQ 0
#define EQ_TYPE_10B 1

#ifndef GSTENGINE_H_
#define GSTENGINE_H_

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "Engine/GStreamer/StreamRecorder.h"
#include "Engine/Engine.h"

#include <gst/gst.h>
#include <gst/gstbuffer.h>

#include <QObject>
#include <QDebug>
#include <QTimer>

#include <vector>

using namespace std;


#define CAPS_TYPE_INT 0
#define CAPS_TYPE_FLOAT 1
#define CAPS_TYPE_UNKNOWN -1
struct MyCaps {
    int type;
    bool sig;
    int width;
    int channels;
    bool is_parsed;


    void parse(GstCaps* caps){
        QString info = gst_caps_to_string(caps);
        //qDebug() << info;

        QStringList lst = info.split(",");
        foreach(QString s, lst){

            s = s.trimmed();
            if(s.startsWith("audio", Qt::CaseInsensitive)){
                if(s.contains("int", Qt::CaseInsensitive)) type = CAPS_TYPE_INT;
                else if(s.contains("float", Qt::CaseInsensitive)) type = CAPS_TYPE_FLOAT;
                else type = CAPS_TYPE_UNKNOWN;
            }

            else if(s.startsWith("signed", Qt::CaseInsensitive)){
                if(s.contains("true", Qt::CaseInsensitive)) sig = true;
                else sig = false;
            }

            else if(s.startsWith("width", Qt::CaseInsensitive)){
                width = s.right(2).toInt();
            }

            else if(s.startsWith("channels", Qt::CaseInsensitive)){
                channels = s.right(1).toInt();
                if(channels > 2) channels = 2;
            }
        }
        is_parsed = true;
    }
};



class GST_Engine : public Engine {

	Q_OBJECT
	Q_INTERFACES(Engine)


public:

	GST_Engine();
	virtual ~GST_Engine();

	virtual void init();


private:



    GstElement* _pipeline;
	GstElement* _equalizer;
    GstElement* _eq_queue;
    GstElement* _volume;

    GstPad* _tee_app_pad;
    GstPad* _app_pad;

	GstElement* _audio_sink;
    GstElement* _audio_bin;
    GstElement* _gio_src;
    GstElement* _decodebin;
	
	GstElement* _app_sink;
	GstElement* _app_queue;
	
    GstElement* _tee;
	
    GstBus*		_bus;
	StreamRecorder* _stream_recorder;

	LastTrack*  _last_track;
    MyCaps     _caps;





private slots:
    virtual void sr_initialized(bool);
    virtual void sr_ended();
    virtual void sr_not_valid();
    void timeout();
    


public slots:
    virtual void play(int pos_sec=0);
	virtual void stop();
	virtual void pause();
	virtual void setVolume(int vol);

	virtual void jump(int where, bool percent=true);
    virtual void changeTrack(const MetaData&, int pos_sec=0, bool start_play=true);
    virtual void changeTrack(const QString&, int pos_sec=0, bool start_play=true );
    virtual void psl_gapless_track(const MetaData&);
	virtual void eq_changed(int, int);
	virtual void eq_enable(bool);
    virtual void psl_new_stream_session();

 	virtual void record_button_toggled(bool);

    virtual void psl_sr_set_active(bool);
    virtual void psl_calc_level(bool);



public:
	// callback -> class
	void		state_changed();
	void		set_cur_position(quint32);
	void		set_track_finished();
    void        set_about_to_finish();
    void        emit_buffer(float inv_array_elements, float scale);
	void 		set_buffer(GstBuffer*);

	virtual void 	load_equalizer(vector<EQ_Setting>&);
	virtual int		getState();
	virtual QString	getName();


private:
	CSettingsStorage* _settings;

	void init_play_pipeline();
    bool set_uri(const MetaData& md, bool& start_play);
    QTimer* _timer;

};



#endif /* GSTENGINE_H_ */
