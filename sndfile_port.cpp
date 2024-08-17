#include <iostream>
#include <ostream>
#include <sndfile.h>
#include <sndfile.hh>
#include "sndfile_port.hpp"
#include "audio_file.hpp"

Sndfile_port::~Sndfile_port() {
    this->close();
}

int Sndfile_port::open(std::string path) {

    this->file = SndfileHandle(path);

    if (file.error() == 0) {
        std::cout << "file info:" << std::endl;
        std::cout << " name : " << path << std::endl;
        std::cout << " channels: " << this->file.channels() << std::endl;
        std::cout << " frames: "  << this->file.frames() << std::endl;
        std::cout << " sample rate:" << this->file.samplerate() << std::endl;

        SF_FORMAT_INFO format_info;
        format_info.format = this->file.format() & SF_FORMAT_TYPEMASK;
        if (this->file.command(SFC_GET_FORMAT_INFO, &format_info, sizeof(format_info)) == 0) {
            std::cout << " type format: " << format_info.format << std::endl;
            std::cout << "   name: "      << (format_info.name != (void*)NULL ? format_info.name : "none") << std::endl;
            std::cout << "   extension: " << (format_info.extension != (void*)NULL ? format_info.extension : "none") << std::endl;
        }
        format_info.format = this->file.format() & SF_FORMAT_SUBMASK;
        if (this->file.command(SFC_GET_FORMAT_SUBTYPE, &format_info, sizeof(format_info)) == 0) {
            std::cout << " sub format: "  << format_info.format << std::endl;
            std::cout << "   name: "      << (format_info.name != (void*)NULL ? format_info.name : "none") << std::endl;
            std::cout << "   extension: " << (format_info.extension != (void*)NULL ? format_info.extension : "none") << std::endl;
        }

        SF_CART_INFO cart;
        std::cout << "cart info:";
        if (file.command(SFC_GET_CART_INFO, &cart, sizeof(cart)) == SF_TRUE) {
            std::cout << "  title: " << cart.title << std::endl;
            std::cout << "  artist: " << cart.artist << std::endl;
        }
        else {
            std::cout << " none" << std::endl;
        }

        SF_CUES cues;
        std::cout << "cues info:";
        if (file.command(SFC_GET_CUE, &cues, sizeof (cues)) == SF_TRUE) {
            while (cues.cue_count > 0) {
                std::cout << std::endl;
                std::cout << "  index: "         << cues.cue_points[cues.cue_count-1].indx << std::endl;
                std::cout << "  name: "          << cues.cue_points[cues.cue_count-1].name << std::endl;
                std::cout << "  position: "      << cues.cue_points[cues.cue_count-1].position << std::endl;
                std::cout << "  chunck fcc: "    << cues.cue_points[cues.cue_count-1].fcc_chunk << std::endl;
                std::cout << "  chunck start: "  << cues.cue_points[cues.cue_count-1].chunk_start << std::endl;
                std::cout << "  block start: "   << cues.cue_points[cues.cue_count-1].block_start << std::endl;
                std::cout << "  sample offset: " << cues.cue_points[cues.cue_count-1].sample_offset << std::endl;
                cues.cue_count--;
            }
        }
        else {
            std::cout << " none" << std::endl;
        }

        this->get_meta();

        switch (this->file.format() & SF_FORMAT_SUBMASK) {
            case SF_FORMAT_PCM_S8: this->format = Audio_port_base::au_port_format::pcm_s8; break;
            case SF_FORMAT_PCM_16: this->format = Audio_port_base::au_port_format::pcm_s16; break;
            case SF_FORMAT_PCM_24: this->format = Audio_port_base::au_port_format::pcm_s24; break;
            case SF_FORMAT_PCM_32: this->format = Audio_port_base::au_port_format::pcm_s32; break;
            case SF_FORMAT_PCM_U8: this->format = Audio_port_base::au_port_format::pcm_u8; break;
            case SF_FORMAT_FLOAT: this->format = Audio_port_base::au_port_format::pcm_float; break;
            default:
                this->format = Audio_port_base::au_port_format::pcm_none;
                std::cout << "error file format not supported" << std::endl;
                return -1;
        }

        switch (this->file.channels()) {
            case 1U: this->channel = Audio_port_base::au_port_channel::channel_mono; break;
            case 2U: this->channel = Audio_port_base::au_port_channel::channel_stereo; break;
            default:
                this->channel = Audio_port_base::au_port_channel::channel_none;
                std::cout << "error file channel not supported" << std::endl;
                return -1;
        }

        switch (this->file.samplerate()) {
            case 8000U:  this->sample_rate = Audio_port_base::au_port_sample_rate::rate_8k; break;
            case 16000U: this->sample_rate = Audio_port_base::au_port_sample_rate::rate_16k; break;
            case 44100U: this->sample_rate = Audio_port_base::au_port_sample_rate::rate_44k1; break;
            case 48000U: this->sample_rate = Audio_port_base::au_port_sample_rate::rate_48k; break;
            case 88200U: this->sample_rate = Audio_port_base::au_port_sample_rate::rate_88k2; break;
            case 96000U: this->sample_rate = Audio_port_base::au_port_sample_rate::rate_96k; break;
            case 192000: this->sample_rate = Audio_port_base::au_port_sample_rate::rate_192k; break;
            default:
                this->sample_rate = Audio_port_base::au_port_sample_rate::rate_none;
                std::cout << "error file sample rate not supported" << std::endl;
                return -1;
        }
    }
    else {
        std::cout << "error: " << file.strError() << std::endl;
        return -1;
    }
    return 0;
}

int Sndfile_port::close() {
    this->file.~SndfileHandle();
    return 0;
}

int Sndfile_port::read(std::uint8_t* pbuffer, std::uint32_t len) {
    sf_count_t byte_read = 0;

    byte_read = this->file.readf(reinterpret_cast<short*>(pbuffer), len);
    if (byte_read < 0) {
        std::cout << "error file read: " << this->file.strError() << std::endl;
    }

    return byte_read;
}

int Sndfile_port::seek(std::int32_t position) {
    sf_count_t max_position;

    max_position = this->file.frames();

    if (position >= max_position) {
        std::cout << "error file seek position out of bound" << std::endl;
        return -1;
    }

    if (this->file.seek(position, SEEK_CUR) < 0) {
        std::cout << "error file seek: " << this->file.strError() << std::endl;
        return -1;
    }
    return 0;
}

int Sndfile_port::get_meta() {
    // Assign meta data of file
    const char* str = NULL;
    std::cout << "meta: " << std::endl;
    // Title
    str = this->file.getString(SF_STR_TITLE);
    this->meta.title.assign((str != NULL ? str : "none"));
    std::cout << "  title: " << this->meta.title << std::endl;
    // Album
    str = this->file.getString(SF_STR_ALBUM);
    this->meta.album.assign((str != NULL ? str : "none"));
    std::cout << "  album: " << this->meta.album << std::endl;
    // Track number
    str = this->file.getString(SF_STR_TRACKNUMBER);
    this->meta.track_number.assign((str != NULL ? str : "none"));
    std::cout << "  track: " << this->meta.track_number << std::endl;
    // Genre
    str = this->file.getString(SF_STR_GENRE);
    this->meta.genre.assign((str != NULL ? str : "none"));
    std::cout << "  genre: " << this->meta.genre << std::endl;
    // Date
    str = this->file.getString(SF_STR_DATE);
    this->meta.date.assign((str != NULL ? str : "none"));
    std::cout << "  date: " << this->meta.date << std::endl;
    // Comment
    str = this->file.getString(SF_STR_COMMENT);
    this->meta.comment.assign((str != NULL ? str : "none"));
    std::cout << "  comment: " << this->meta.comment << std::endl;
    // Copyright
    str = this->file.getString(SF_STR_COPYRIGHT);
    this->meta.copyright.assign((str != NULL ? str : "none"));
    std::cout << "  copyright: " << this->meta.copyright << std::endl;
    // License
    str = this->file.getString(SF_STR_LICENSE);
    this->meta.license.assign((str != NULL ? str : "none"));
    std::cout << "  license: " << this->meta.license << std::endl;
    // Software
    str = this->file.getString(SF_STR_SOFTWARE);
    this->meta.software.assign((str != NULL ? str : "none"));
    std::cout << "  software: " << this->meta.software << std::endl;

    return 0;
}
