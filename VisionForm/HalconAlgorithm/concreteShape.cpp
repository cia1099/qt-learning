///////////////////////////////////////////////////////////////////////////////
// File generated by HDevelop for HALCON/C++ Version 18.05.0.1
///////////////////////////////////////////////////////////////////////////////



#ifndef __APPLE__
#  include "HalconCpp.h"
#  include "HDevThread.h"
#  if defined(__linux__) && !defined(__arm__) && !defined(NO_EXPORT_APP_MAIN)
#    include <X11/Xlib.h>
#  endif
#else
#  ifndef HC_LARGE_IMAGES
#    include <HALCONCpp/HalconCpp.h>
#    include <HALCONCpp/HDevThread.h>
#  else
#    include <HALCONCppxl/HalconCpp.h>
#    include <HALCONCppxl/HDevThread.h>
#  endif
#  include <stdio.h>
#  include <HALCON/HpThread.h>
#  include <CoreFoundation/CFRunLoop.h>
#endif



using namespace HalconCpp;

// Procedure declarations 
// External procedures 
// Chapter: Develop
// Short Description: Open a new graphics window that preserves the aspect ratio of the given image. 
void dev_open_window_fit_image (HObject ho_Image, HTuple hv_Row, HTuple hv_Column, 
    HTuple hv_WidthLimit, HTuple hv_HeightLimit, HTuple *hv_WindowHandle);
// Chapter: Develop
// Short Description: Switch dev_update_pc, dev_update_var and dev_update_window to 'off'. 
void dev_update_off ();
// Chapter: Graphics / Text
// Short Description: Set font independent of OS 
void set_display_font (HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, 
    HTuple hv_Slant);

// Procedures 
// External procedures 
// Chapter: Develop
// Short Description: Open a new graphics window that preserves the aspect ratio of the given image. 
void dev_open_window_fit_image (HObject ho_Image, HTuple hv_Row, HTuple hv_Column, 
    HTuple hv_WidthLimit, HTuple hv_HeightLimit, HTuple *hv_WindowHandle)
{

  // Local iconic variables

  // Local control variables
  HTuple  hv_MinWidth, hv_MaxWidth, hv_MinHeight;
  HTuple  hv_MaxHeight, hv_ResizeFactor, hv_ImageWidth, hv_ImageHeight;
  HTuple  hv_TempWidth, hv_TempHeight, hv_WindowWidth, hv_WindowHeight;

  //This procedure opens a new graphics window and adjusts the size
  //such that it fits into the limits specified by WidthLimit
  //and HeightLimit, but also maintains the correct image aspect ratio.
  //
  //If it is impossible to match the minimum and maximum extent requirements
  //at the same time (f.e. if the image is very long but narrow),
  //the maximum value gets a higher priority,
  //
  //Parse input tuple WidthLimit
  if (0 != (HTuple((hv_WidthLimit.TupleLength())==0).TupleOr(hv_WidthLimit<0)))
  {
    hv_MinWidth = 500;
    hv_MaxWidth = 800;
  }
  else if (0 != ((hv_WidthLimit.TupleLength())==1))
  {
    hv_MinWidth = 0;
    hv_MaxWidth = hv_WidthLimit;
  }
  else
  {
    hv_MinWidth = ((const HTuple&)hv_WidthLimit)[0];
    hv_MaxWidth = ((const HTuple&)hv_WidthLimit)[1];
  }
  //Parse input tuple HeightLimit
  if (0 != (HTuple((hv_HeightLimit.TupleLength())==0).TupleOr(hv_HeightLimit<0)))
  {
    hv_MinHeight = 400;
    hv_MaxHeight = 600;
  }
  else if (0 != ((hv_HeightLimit.TupleLength())==1))
  {
    hv_MinHeight = 0;
    hv_MaxHeight = hv_HeightLimit;
  }
  else
  {
    hv_MinHeight = ((const HTuple&)hv_HeightLimit)[0];
    hv_MaxHeight = ((const HTuple&)hv_HeightLimit)[1];
  }
  //
  //Test, if window size has to be changed.
  hv_ResizeFactor = 1;
  GetImageSize(ho_Image, &hv_ImageWidth, &hv_ImageHeight);
  //First, expand window to the minimum extents (if necessary).
  if (0 != (HTuple(hv_MinWidth>hv_ImageWidth).TupleOr(hv_MinHeight>hv_ImageHeight)))
  {
    hv_ResizeFactor = (((hv_MinWidth.TupleReal())/hv_ImageWidth).TupleConcat((hv_MinHeight.TupleReal())/hv_ImageHeight)).TupleMax();
  }
  hv_TempWidth = hv_ImageWidth*hv_ResizeFactor;
  hv_TempHeight = hv_ImageHeight*hv_ResizeFactor;
  //Then, shrink window to maximum extents (if necessary).
  if (0 != (HTuple(hv_MaxWidth<hv_TempWidth).TupleOr(hv_MaxHeight<hv_TempHeight)))
  {
    hv_ResizeFactor = hv_ResizeFactor*((((hv_MaxWidth.TupleReal())/hv_TempWidth).TupleConcat((hv_MaxHeight.TupleReal())/hv_TempHeight)).TupleMin());
  }
  hv_WindowWidth = hv_ImageWidth*hv_ResizeFactor;
  hv_WindowHeight = hv_ImageHeight*hv_ResizeFactor;
  //Resize window
  SetWindowAttr("background_color","black");
  OpenWindow(hv_Row,hv_Column,hv_WindowWidth,hv_WindowHeight,0,"visible","",&(*hv_WindowHandle));
  HDevWindowStack::Push((*hv_WindowHandle));
  if (HDevWindowStack::IsOpen())
    SetPart(HDevWindowStack::GetActive(),0, 0, hv_ImageHeight-1, hv_ImageWidth-1);
  return;
}

// Chapter: Develop
// Short Description: Switch dev_update_pc, dev_update_var and dev_update_window to 'off'. 
void dev_update_off ()
{

  //This procedure sets different update settings to 'off'.
  //This is useful to get the best performance and reduce overhead.
  //
  // dev_update_pc(...); only in hdevelop
  // dev_update_var(...); only in hdevelop
  // dev_update_window(...); only in hdevelop
  return;
}

// Chapter: Graphics / Text
// Short Description: Set font independent of OS 
void set_display_font (HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, 
    HTuple hv_Slant)
{

  // Local iconic variables

  // Local control variables
  HTuple  hv_OS, hv_Fonts, hv_Style, hv_Exception;
  HTuple  hv_AvailableFonts, hv_Fdx, hv_Indices;

  //This procedure sets the text font of the current window with
  //the specified attributes.
  //
  //Input parameters:
  //WindowHandle: The graphics window for which the font will be set
  //Size: The font size. If Size=-1, the default of 16 is used.
  //Bold: If set to 'true', a bold font is used
  //Slant: If set to 'true', a slanted font is used
  //
  GetSystem("operating_system", &hv_OS);
  if (0 != (HTuple(hv_Size==HTuple()).TupleOr(hv_Size==-1)))
  {
    hv_Size = 16;
  }
  if (0 != ((hv_OS.TupleSubstr(0,2))==HTuple("Win")))
  {
    //Restore previous behaviour
    hv_Size = (1.13677*hv_Size).TupleInt();
  }
  else
  {
    hv_Size = hv_Size.TupleInt();
  }
  if (0 != (hv_Font==HTuple("Courier")))
  {
    hv_Fonts.Clear();
    hv_Fonts[0] = "Courier";
    hv_Fonts[1] = "Courier 10 Pitch";
    hv_Fonts[2] = "Courier New";
    hv_Fonts[3] = "CourierNew";
    hv_Fonts[4] = "Liberation Mono";
  }
  else if (0 != (hv_Font==HTuple("mono")))
  {
    hv_Fonts.Clear();
    hv_Fonts[0] = "Consolas";
    hv_Fonts[1] = "Menlo";
    hv_Fonts[2] = "Courier";
    hv_Fonts[3] = "Courier 10 Pitch";
    hv_Fonts[4] = "FreeMono";
    hv_Fonts[5] = "Liberation Mono";
  }
  else if (0 != (hv_Font==HTuple("sans")))
  {
    hv_Fonts.Clear();
    hv_Fonts[0] = "Luxi Sans";
    hv_Fonts[1] = "DejaVu Sans";
    hv_Fonts[2] = "FreeSans";
    hv_Fonts[3] = "Arial";
    hv_Fonts[4] = "Liberation Sans";
  }
  else if (0 != (hv_Font==HTuple("serif")))
  {
    hv_Fonts.Clear();
    hv_Fonts[0] = "Times New Roman";
    hv_Fonts[1] = "Luxi Serif";
    hv_Fonts[2] = "DejaVu Serif";
    hv_Fonts[3] = "FreeSerif";
    hv_Fonts[4] = "Utopia";
    hv_Fonts[5] = "Liberation Serif";
  }
  else
  {
    hv_Fonts = hv_Font;
  }
  hv_Style = "";
  if (0 != (hv_Bold==HTuple("true")))
  {
    hv_Style += HTuple("Bold");
  }
  else if (0 != (hv_Bold!=HTuple("false")))
  {
    hv_Exception = "Wrong value of control parameter Bold";
    throw HException(hv_Exception);
  }
  if (0 != (hv_Slant==HTuple("true")))
  {
    hv_Style += HTuple("Italic");
  }
  else if (0 != (hv_Slant!=HTuple("false")))
  {
    hv_Exception = "Wrong value of control parameter Slant";
    throw HException(hv_Exception);
  }
  if (0 != (hv_Style==HTuple("")))
  {
    hv_Style = "Normal";
  }
  QueryFont(hv_WindowHandle, &hv_AvailableFonts);
  hv_Font = "";
  {
  HTuple end_val48 = (hv_Fonts.TupleLength())-1;
  HTuple step_val48 = 1;
  for (hv_Fdx=0; hv_Fdx.Continue(end_val48, step_val48); hv_Fdx += step_val48)
  {
    hv_Indices = hv_AvailableFonts.TupleFind(HTuple(hv_Fonts[hv_Fdx]));
    if (0 != ((hv_Indices.TupleLength())>0))
    {
      if (0 != (HTuple(hv_Indices[0])>=0))
      {
        hv_Font = HTuple(hv_Fonts[hv_Fdx]);
        break;
      }
    }
  }
  }
  if (0 != (hv_Font==HTuple("")))
  {
    throw HException("Wrong value of control parameter Font");
  }
  hv_Font = (((hv_Font+"-")+hv_Style)+"-")+hv_Size;
  SetFont(hv_WindowHandle, hv_Font);
  return;
}

#ifndef NO_EXPORT_MAIN
// Main procedure 
void action()
{

  // Local iconic variables
  HObject  ho_image, ho_ROI_c1, ho_ROI_c2, ho_ROI_line;
  HObject  ho_ModelContour, ho_MeasureContour, ho_EmptyObject;
  HObject  ho_Contours, ho_UsedEdges, ho_ResultContours, ho_resultPts;
  HObject  ho_fitContour;

  // Local control variables
  HTuple  hv_Width, hv_Height, hv_y_c1, hv_x_c1;
  HTuple  hv_r_c1, hv_y_c2, hv_x_c2, hv_r_c2, hv_pt1_y, hv_pt1_x;
  HTuple  hv_pt2_y, hv_pt2_x, hv_Nr, hv_Nc, hv_Dist, hv_MetrologyHandle;
  HTuple  hv_margin, hv_conf, hv_circles, hv_line, hv_c_idx;
  HTuple  hv_l_idx, hv_Row, hv_Column, hv_Row1, hv_Column1;
  HTuple  hv_UsedRow, hv_UsedColumn, hv_mY, hv_mX, hv_mL;

  //dev_update_off ()
  ReadImage(&ho_image, "./concrete.bmp");
  Rgb1ToGray(ho_image, &ho_image);
  GetImageSize(ho_image, &hv_Width, &hv_Height);
  //dev_close_window ()
  //dev_open_window_fit_image (image, 0, 0, 330, 500, WindowHandle)
  //* set_display_font (WindowHandle, 16, 'mono', 'true', 'false')
  //dev_display (image)
  GenCircleContourXld(&ho_ROI_c1, 869.276, 749.49, 339.453, HTuple(80.205).TupleRad(), 
      HTuple(282.462).TupleRad(), "positive", 1);
  GenCircleContourXld(&ho_ROI_c2, 1042.68, 1697.75, 350.594, HTuple(109.884).TupleRad(), 
      HTuple(-133.277).TupleRad(), "negative", 1);
  GenContourPolygonXld(&ho_ROI_line, (HTuple(721.256).Append(519.993)), (HTuple(1892.6).Append(726.48)));
  SmallestCircleXld(ho_ROI_c1, &hv_y_c1, &hv_x_c1, &hv_r_c1);
  SmallestCircleXld(ho_ROI_c2, &hv_y_c2, &hv_x_c2, &hv_r_c2);
  FitLineContourXld(ho_ROI_line, "tukey", -1, 0, 5, 2, &hv_pt1_y, &hv_pt1_x, &hv_pt2_y, 
      &hv_pt2_x, &hv_Nr, &hv_Nc, &hv_Dist);

  CreateMetrologyModel(&hv_MetrologyHandle);
  SetMetrologyModelImageSize(hv_MetrologyHandle, hv_Width, hv_Height);
  hv_margin = 20;
  hv_conf = 0.5;
  hv_circles.Clear();
  hv_circles.Append(hv_y_c1);
  hv_circles.Append(hv_x_c1);
  hv_circles.Append(hv_r_c1);
  hv_circles.Append(hv_y_c2);
  hv_circles.Append(hv_x_c2);
  hv_circles.Append(hv_r_c2);
  hv_line.Clear();
  hv_line.Append(hv_pt1_y);
  hv_line.Append(hv_pt1_x);
  hv_line.Append(hv_pt2_y);
  hv_line.Append(hv_pt2_x);
  AddMetrologyObjectGeneric(hv_MetrologyHandle, "circle", hv_circles, hv_margin, 
      5, 1, 30, HTuple(), HTuple(), &hv_c_idx);
  AddMetrologyObjectGeneric(hv_MetrologyHandle, "line", hv_line, hv_margin, 5, 1, 
      30, HTuple(), HTuple(), &hv_l_idx);
  //Inspect the shapes that have been added to the metrology model
  GetMetrologyObjectModelContour(&ho_ModelContour, hv_MetrologyHandle, hv_l_idx, 
      1.5);
  GetMetrologyObjectMeasures(&ho_MeasureContour, hv_MetrologyHandle, hv_l_idx, "all", 
      &hv_Row, &hv_Column);
  GenEmptyObj(&ho_EmptyObject);
  //show_contours (image, ModelContour, MeasureContour, EmptyObject, WindowHandle, '')
  //stop ()
  SetMetrologyObjectParam(hv_MetrologyHandle, "all", "min_score", hv_conf);
  SetMetrologyObjectParam(hv_MetrologyHandle, "all", "measure_transition", "uniform");
  SetMetrologyObjectParam(hv_MetrologyHandle, "all", "num_instances", 1);
  ApplyMetrologyModel(ho_image, hv_MetrologyHandle);
  //Get measure regions for visualization
  GetMetrologyObjectMeasures(&ho_Contours, hv_MetrologyHandle, "all", "all", &hv_Row1, 
      &hv_Column1);
  //gen_cross_contour_xld (Cross, Row1, Column1, 10, rad(135))
  //Get the edge points that were actually used to fit the geometric forms
  GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "used_edges", "row", 
      &hv_UsedRow);
  GetMetrologyObjectResult(hv_MetrologyHandle, "all", "all", "used_edges", "column", 
      &hv_UsedColumn);
  GenCrossContourXld(&ho_UsedEdges, hv_UsedRow, hv_UsedColumn, 10, HTuple(45).TupleRad());
  //Get a visualization of the measured geometric forms
  GetMetrologyObjectResultContour(&ho_ResultContours, hv_MetrologyHandle, "all", 
      "all", 1.5);
  GetMetrologyObjectResult(hv_MetrologyHandle, hv_c_idx, "all", "result_type", "row", 
      &hv_mY);
  GetMetrologyObjectResult(hv_MetrologyHandle, hv_c_idx, "all", "result_type", "column", 
      &hv_mX);
  GetMetrologyObjectResult(hv_MetrologyHandle, hv_l_idx, "all", "result_type", "all_param", 
      &hv_mL);
  hv_mY = hv_mY.TupleConcat(HTuple(hv_mL[HTuple::TupleGenSequence(0,3,2)]));
  hv_mX = hv_mX.TupleConcat(HTuple(hv_mL[HTuple::TupleGenSequence(1,3,2)]));
  GenCrossContourXld(&ho_resultPts, hv_mY, hv_mX, 10, HTuple(135).TupleRad());

  //dev_display (image)
  //dev_set_line_width (1)
  //dev_set_color ('green')
  //dev_display (ResultContours)
  //dev_set_line_width (1)
  //dev_set_color ('red')
  //dev_display (UsedEdges)

  //dev_set_color ('cyan')
  //dev_display (Cross)
  //dev_set_line_width (5)
  //dev_display (resultPts)

  ClearMetrologyModel(hv_MetrologyHandle);

  // stop(...); only in hdevelop
  GenContourNurbsXld(&ho_fitContour, hv_UsedRow, hv_UsedColumn, "auto", "auto", 3, 
      1, 5);
  //dev_set_color ('magenta')
  //dev_set_line_width (1)
  //dev_display (fitContour)




}


#ifndef NO_EXPORT_APP_MAIN

#ifdef __APPLE__
// On OS X systems, we must have a CFRunLoop running on the main thread in
// order for the HALCON graphics operators to work correctly, and run the
// action function in a separate thread. A CFRunLoopTimer is used to make sure
// the action function is not called before the CFRunLoop is running.
// Note that starting with macOS 10.12, the run loop may be stopped when a
// window is closed, so we need to put the call to CFRunLoopRun() into a loop
// of its own.
HTuple      gStartMutex;
H_pthread_t gActionThread;
HBOOL       gTerminate = FALSE;

static void timer_callback(CFRunLoopTimerRef timer, void *info)
{
  UnlockMutex(gStartMutex);
}

static Herror apple_action(void **parameters)
{
  // Wait until the timer has fired to start processing.
  LockMutex(gStartMutex);
  UnlockMutex(gStartMutex);

  try
  {
    action();
  }
  catch (HException &exception)
  {
    fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
            (const char *)exception.ProcName(),
            (const char *)exception.ErrorMessage());
  }

  // Tell the main thread to terminate itself.
  LockMutex(gStartMutex);
  gTerminate = TRUE;
  UnlockMutex(gStartMutex);
  CFRunLoopStop(CFRunLoopGetMain());
  return H_MSG_OK;
}

static int apple_main(int argc, char *argv[])
{
  Herror                error;
  CFRunLoopTimerRef     Timer;
  CFRunLoopTimerContext TimerContext = { 0, 0, 0, 0, 0 };

  CreateMutex("type","sleep",&gStartMutex);
  LockMutex(gStartMutex);

  error = HpThreadHandleAlloc(&gActionThread);
  if (H_MSG_OK != error)
  {
    fprintf(stderr,"HpThreadHandleAlloc failed: %d\n", error);
    exit(1);
  }

  error = HpThreadCreate(gActionThread,0,apple_action);
  if (H_MSG_OK != error)
  {
    fprintf(stderr,"HpThreadCreate failed: %d\n", error);
    exit(1);
  }

  Timer = CFRunLoopTimerCreate(kCFAllocatorDefault,
                               CFAbsoluteTimeGetCurrent(),0,0,0,
                               timer_callback,&TimerContext);
  if (!Timer)
  {
    fprintf(stderr,"CFRunLoopTimerCreate failed\n");
    exit(1);
  }
  CFRunLoopAddTimer(CFRunLoopGetCurrent(),Timer,kCFRunLoopCommonModes);

  for (;;)
  {
    HBOOL terminate;

    CFRunLoopRun();

    LockMutex(gStartMutex);
    terminate = gTerminate;
    UnlockMutex(gStartMutex);

    if (terminate)
      break;
  }

  CFRunLoopRemoveTimer(CFRunLoopGetCurrent(),Timer,kCFRunLoopCommonModes);
  CFRelease(Timer);

  error = HpThreadHandleFree(gActionThread);
  if (H_MSG_OK != error)
  {
    fprintf(stderr,"HpThreadHandleFree failed: %d\n", error);
    exit(1);
  }

  ClearMutex(gStartMutex);
  return 0;
}
#endif

int main(int argc, char *argv[])
{
  int ret = 0;

  try
  {
#if defined(_WIN32)
    SetSystem("use_window_thread", "true");
#elif defined(__linux__) && !defined(__arm__)
    XInitThreads();
#endif

    // Default settings used in HDevelop (can be omitted) 
    SetSystem("width", 512);
    SetSystem("height", 512);

#ifndef __APPLE__
    action();
#else
    ret = apple_main(argc,argv);
#endif
  }
  catch (HException &exception)
  {
    fprintf(stderr,"  Error #%u in %s: %s\n", exception.ErrorCode(),
            (const char *)exception.ProcName(),
            (const char *)exception.ErrorMessage());
    ret = 1;
  }
  return ret;
}

#endif


#endif


