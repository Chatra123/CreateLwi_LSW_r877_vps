


/* This file is available under an ISC license.
 * However, when distributing its binary file, it will be under LGPL or GPL. */

#define NO_PROGRESS_HANDLER

/* Libav (LGPL or GPL) */
#include <libavformat/avformat.h>       /* Codec specific info importer */
#include <libavcodec/avcodec.h>         /* Decoder */
#include <libswscale/swscale.h>         /* Colorspace converter */
#include <libavutil/imgutils.h>

/* Dummy definitions.
 * Audio resampler/buffer is NOT used at all in this filter. */
typedef void AVAudioResampleContext;
typedef void audio_samples_t;
//int flush_resampler_buffers( AVAudioResampleContext *avr ){ return 0; }
//int update_resampler_configuration( AVAudioResampleContext *avr,
//                                    uint64_t out_channel_layout, int out_sample_rate, enum AVSampleFormat out_sample_fmt,
//                                    uint64_t  in_channel_layout, int  in_sample_rate, enum AVSampleFormat  in_sample_fmt,
//                                    int *input_planes, int *input_block_align ){ return 0; }
//int resample_audio( AVAudioResampleContext *avr, audio_samples_t *out, audio_samples_t *in ){ return 0; }
#include "../common/audio_output.h"
//uint64_t output_pcm_samples_from_buffer
//(
//    lw_audio_output_handler_t *aohp,
//    AVFrame                   *frame_buffer,
//    uint8_t                  **output_buffer,
//    enum audio_output_flag    *output_flags
//)
//{
//    return 0;
//}

//uint64_t output_pcm_samples_from_packet
//(
//    lw_audio_output_handler_t *aohp,
//    AVCodecContext            *ctx,
//    AVPacket                  *pkt,
//    AVFrame                   *frame_buffer,
//    uint8_t                  **output_buffer,
//    enum audio_output_flag    *output_flags
//)
//{
//    return 0;
//}

//void lw_cleanup_audio_output_handler( lw_audio_output_handler_t *aohp ){ }

#include "lsmashsource.h"
#include "video_output.h"

#include "../common/progress.h"
#include "../common/lwlibav_dec.h"
#include "../common/lwlibav_video.h"
#include "../common/lwlibav_audio.h"
#include "../common/lwindex.h"


typedef struct
{
    VSVideoInfo                     vi;
    lwlibav_file_handler_t          lwh;
    lwlibav_video_decode_handler_t *vdhp;
    lwlibav_video_output_handler_t *vohp;
    lwlibav_audio_decode_handler_t *adhp;
    lwlibav_audio_output_handler_t *aohp;
    char preferred_decoder_names_buf[PREFERRED_DECODER_NAMES_BUFSIZE];
} lwlibav_handler_t;



/* Deallocate the handler of this plugin. */
static void free_handler
(
    lwlibav_handler_t **hpp
)
{
    if( !hpp || !*hpp )
        return;
    lwlibav_handler_t *hp = *hpp;
    lw_free( lwlibav_video_get_preferred_decoder_names( hp->vdhp ) );
    lwlibav_video_free_decode_handler( hp->vdhp );
    lwlibav_video_free_output_handler( hp->vohp );
    lwlibav_audio_free_decode_handler( hp->adhp );
    lwlibav_audio_free_output_handler( hp->aohp );
    lw_free( hp->lwh.file_path );
    lw_free( hp );
}


/* Allocate the handler of this plugin. */
static lwlibav_handler_t *alloc_handler
(
    void
)
{
    lwlibav_handler_t *hp = (lwlibav_handler_t*)lw_malloc_zero( sizeof(lwlibav_handler_t) );
    if( !hp )
        return NULL;
    if( !(hp->vdhp = lwlibav_video_alloc_decode_handler())
     || !(hp->vohp = lwlibav_video_alloc_output_handler())
     || !(hp->adhp = lwlibav_audio_alloc_decode_handler())
     || !(hp->aohp = lwlibav_audio_alloc_output_handler()) )
    {
        free_handler( &hp );
        return NULL;
    }
    return hp;
}



//static void VS_CC vs_filter_free( void *instance_data, VSCore *core, const VSAPI *vsapi )
//{
//    free_handler( (lwlibav_handler_t **)&instance_data );
//}

int VS_CC vs_lwlibavsource_create_crlwi( const VSMap *in, VSMap *out, void *user_data, VSCore *core, const VSAPI *vsapi,
                                         crlwi_setting_handler__CrLwi *clshp)
{
    /* Get file path. */
//    const char *file_path = vsapi->propGetData( in, "source", 0, NULL );
//    if( !file_path )
//    {
//        vsapi->setError( out, "lsmas: failed to get source file name." );
//        return;
//    }
    /* Allocate the handler of this filter function. */
    lwlibav_handler_t *hp = alloc_handler();
    if( !hp )
    {
//        vsapi->setError( out, "lsmas: failed to allocate the LW-Libav handler." );
        return 1;
    }
    lwlibav_file_handler_t         *lwhp = &hp->lwh;
    lwlibav_video_decode_handler_t *vdhp = hp->vdhp;
    lwlibav_video_output_handler_t *vohp = hp->vohp;
//   vs_video_output_handler_t *vs_vohp = vs_allocate_video_output_handler( vohp );
////    if( !vs_vohp )
////    {
////        free_handler( &hp );
////        vsapi->setError( out, "lsmas: failed to allocate the VapourSynth video output handler." );
////        return;
////    }
////    vohp->private_handler      = vs_vohp;
//
//    vohp->private_handler      = NULL;
//    vohp->free_private_handler = lw_free;
    /* Set up VapourSynth error handler. */
    vs_basic_handler_t vsbh = { 0 };
    vsbh.out       = out;
    vsbh.frame_ctx = NULL;
    vsbh.vsapi     = vsapi;
    /* Set up log handler. */
    lw_log_handler_t lh = { 0 };
    lh.level    = LW_LOG_FATAL;
    lh.priv     = &vsbh;
    lh.show_log = set_error;
    lh.show_log = NULL;                  //crlwi
    /* Get options. */
//    int64_t stream_index;
//    int64_t threads;
//    int64_t cache_index;
//    int64_t seek_mode;
//    int64_t seek_threshold;
//    int64_t variable_info;
//    int64_t direct_rendering;
//    int64_t fps_num;
//    int64_t fps_den;
//    int64_t apply_repeat_flag;
//    int64_t field_dominance;
//    const char *format;
//    const char *preferred_decoder_names;
//    set_option_int64 ( &stream_index,           -1,    "stream_index",   in, vsapi );
//    set_option_int64 ( &threads,                 0,    "threads",        in, vsapi );
//    set_option_int64 ( &cache_index,             1,    "cache",          in, vsapi );
//    set_option_int64 ( &seek_mode,               0,    "seek_mode",      in, vsapi );
//    set_option_int64 ( &seek_threshold,          10,   "seek_threshold", in, vsapi );
//    set_option_int64 ( &variable_info,           0,    "variable",       in, vsapi );
//    set_option_int64 ( &direct_rendering,        0,    "dr",             in, vsapi );
//    set_option_int64 ( &fps_num,                 0,    "fpsnum",         in, vsapi );
//    set_option_int64 ( &fps_den,                 1,    "fpsden",         in, vsapi );
//    set_option_int64 ( &apply_repeat_flag,       0,    "repeat",         in, vsapi );
//    set_option_int64 ( &field_dominance,         0,    "dominance",      in, vsapi );
//    set_option_string( &format,                  NULL, "format",         in, vsapi );
//    set_option_string( &preferred_decoder_names, NULL, "decoder",        in, vsapi );
//    set_preferred_decoder_names_on_buf( hp->preferred_decoder_names_buf, preferred_decoder_names );
//    /* Set options. */
//    lwlibav_option_t opt;
//    opt.file_path         = file_path;
//    opt.threads           = threads >= 0 ? threads : 0;
//    opt.av_sync           = 0;
//    opt.no_create_index   = !cache_index;
//    opt.force_video       = (stream_index >= 0);
//    opt.force_video_index = stream_index >= 0 ? stream_index : -1;
//    opt.force_audio       = 0;
//    opt.force_audio_index = -1;
//    opt.apply_repeat_flag = apply_repeat_flag;
//    opt.field_dominance   = CLIP_VALUE( field_dominance, 0, 2 );    /* 0: Obey source flags, 1: TFF, 2: BFF */
//    opt.vfr2cfr.active    = fps_num > 0 && fps_den > 0 ? 1 : 0;
//    opt.vfr2cfr.fps_num   = fps_num;
//    opt.vfr2cfr.fps_den   = fps_den;

    //crlwi
    /* Set options. */
    lwlibav_option_t opt;
    opt.file_path         = "";
    opt.threads           = 0;
    opt.av_sync           = 0;
    opt.no_create_index   = 0;
    opt.force_video       = 0;
    opt.force_video_index = 0;
    opt.force_audio       = 0;
    opt.force_audio_index = -1;
    opt.apply_repeat_flag = 1;
    opt.field_dominance   = 0;    /* 0: Obey source flags, 1: TFF, 2: BFF */
    opt.vfr2cfr.active    = 0;
    opt.vfr2cfr.fps_num   = 0;
    opt.vfr2cfr.fps_den   = 1;


//    lwlibav_video_set_seek_mode              ( vdhp, CLIP_VALUE( seek_mode,      0, 2 ) );
//    lwlibav_video_set_forward_seek_threshold ( vdhp, CLIP_VALUE( seek_threshold, 1, 999 ) );
//    lwlibav_video_set_preferred_decoder_names( vdhp, tokenize_preferred_decoder_names( hp->preferred_decoder_names_buf ) );
//    vs_vohp->variable_info          = CLIP_VALUE( variable_info,     0, 1 );
//    vs_vohp->direct_rendering       = CLIP_VALUE( direct_rendering,  0, 1 ) && !format;
//    vs_vohp->vs_output_pixel_format = vs_vohp->variable_info ? pfNone : get_vs_output_pixel_format( format );
    /* Set up progress indicator. */
    progress_indicator_t indicator;
    indicator.open   = NULL;
    indicator.update = NULL;
    indicator.close  = NULL;
    /* Construct index. */
    int ret = lwlibav_construct_index__CrLwi( lwhp, vdhp, vohp, hp->adhp, hp->aohp, &lh, &opt, &indicator, NULL, clshp);
    lwlibav_audio_free_decode_handler_ptr( &hp->adhp );
    lwlibav_audio_free_output_handler_ptr( &hp->aohp );
    if( ret < 0 )
    {
//        vs_filter_free( hp, core, vsapi );
//        set_error_on_init( out, vsapi, "lsmas: failed to construct index." );
        return 1;
    }
//    /* Get the desired video track. */
//    lwlibav_video_set_log_handler( vdhp, &lh );
//    if( lwlibav_video_get_desired_track( lwhp->file_path, vdhp, lwhp->threads ) < 0 )
//    {
//        vs_filter_free( hp, core, vsapi );
//        return;
//    }
//    /* Set average framerate. */
//    hp->vi.numFrames = vohp->frame_count;
//    hp->vi.fpsNum    = 25;
//    hp->vi.fpsDen    = 1;
//    lwlibav_video_setup_timestamp_info( lwhp, vdhp, vohp, &hp->vi.fpsNum, &hp->vi.fpsDen );
//    /* Set up decoders for this stream. */
//    if( prepare_video_decoding( hp, out, core, vsapi ) < 0 )
//    {
//        vs_filter_free( hp, core, vsapi );
//        return;
//    }
//    vsapi->createFilter( in, out, "LWLibavSource", vs_filter_init, vs_filter_get_frame, vs_filter_free, fmSerial, 0, hp, core );

    return 0;
}


//-----------------------------------------------------------------------------
//Å™ lwlibav_source.c is above
//-----------------------------------------------------------------------------


void ParseCommandLine(int argc, char** argv,
                      crlwi_setting_handler__CrLwi *clshp)
{
  char file_path[_MAX_PATH] = {0};
  char lwi_path[_MAX_PATH] = {0};
  char file_path_inner_lwi[_MAX_PATH] = {0};
  bool mode_pipe_input = false;
  bool is_filename_inner_lwi = false;
  bool create_footer = false;
  double read_limit_MiBsec = 0.0;


  //parser
  for (size_t i = 1; i < argc; i++)
  {
    //Is argv[1] filepath ?
    if (i == 1)
    {
      FILE *fp = fopen(argv[1], "r");
      if (fp != NULL)
      {
        mode_pipe_input = false;
        sprintf(file_path, "%s", argv[i]);
        fclose(fp);
      }
    }

    //-file
    if (_stricmp(argv[i], "-file") == 0)
    {
      mode_pipe_input = false;
      if (i + 1 < argc)
        sprintf(file_path, "%s", argv[i + 1]);
    }
    //-pipe
    else if (_stricmp(argv[i], "-pipe") == 0)
    {
      mode_pipe_input = true;
      if (i + 1 < argc)
        sprintf(file_path, "%s", argv[i + 1]);
    }
    //-lwi
    else if (_stricmp(argv[i], "-lwi") == 0)
    {
      if (i + 1 < argc)
        sprintf(lwi_path, "%s", argv[i + 1]);
    }
    //-footer
    else if (_stricmp(argv[i], "-footer") == 0)
    {
      create_footer = true;
    }
    //-ref_filename
    else if (_stricmp(argv[i], "-ref_filename") == 0)
    {
      is_filename_inner_lwi = true;
    }
    //-limit
    else if (_stricmp(argv[i], "-limit") == 0)
    {
      if (i + 1 < argc)
        if ((_snscanf(argv[i + 1], 6, "%lf", &read_limit_MiBsec)) <= 0)
          read_limit_MiBsec = 0;
    }
  }



  //parser post process
  {
    //create lwipath from filepath
    if (_stricmp(lwi_path, "") == 0)
      sprintf(lwi_path, "%s.lwi", file_path);

    //append .lwi extension
    char ext[_MAX_EXT];
    _splitpath(lwi_path, NULL, NULL, NULL, ext);
    if (_stricmp(ext, ".lwi") != 0)
    {
      char tmp[_MAX_PATH] = {0};
      sprintf(tmp, "%s.lwi", lwi_path);
      sprintf(lwi_path, "%s", tmp);
    }

    if (is_filename_inner_lwi)
    {
      //fullpath  Å®  filename
      char name[_MAX_PATH], ext[_MAX_EXT];
      _splitpath(file_path, NULL, NULL, name, ext);
      sprintf(file_path_inner_lwi, "%s%s", name, ext);
    }
    else
      sprintf(file_path_inner_lwi, "%s", file_path);
  }

  //parse result
  {
    memset(clshp->file_path, 0, _MAX_PATH);
    memset(clshp->lwi_path, 0, _MAX_PATH);
    memset(clshp->file_path_inner_lwi, 0, _MAX_PATH);
    sprintf(clshp->file_path, "%s", file_path);
    sprintf(clshp->lwi_path, "%s", lwi_path);
    sprintf(clshp->file_path_inner_lwi, "%s", file_path_inner_lwi);
    clshp->mode_pipe_input = mode_pipe_input;
    clshp->create_footer = mode_pipe_input && create_footer;
    clshp->read_limit_MiBsec = read_limit_MiBsec;
  }
}


/*
 * lwindex.c
 *   line 3200    static void create_index__crlwi(...)
 *
 *
 *
*/
int main(int argc, char** argv)
{
// test args
//  char *test_argv[16];
//  test_argv[0] = "app_path";
//  test_argv[1] = "-file";
//  test_argv[2] = "E:/Test_lwi/a13m.ts";
//  argv = test_argv;
//  argc = 3;


  crlwi_setting_handler__CrLwi clshp;
  ParseCommandLine(argc, argv, &clshp);

  //check file existance
  if (clshp.mode_pipe_input == false)
  {
    FILE *fp = fopen(clshp.file_path, "r");
    if (fp == NULL)
      return 1;
    fclose(fp);
  }


	int ret_code = vs_lwlibavsource_create_crlwi( NULL, NULL, NULL, NULL, NULL, &clshp);

	return ret_code;
}

