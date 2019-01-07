# ObjectDetectionTracking
Ard Object Detection and Tracking Over Bluetooth

Introduction:
This is an Android application specifically designed for students and electronics hobbyist. It uses OpenCV libararies for computer vision detection.

The application can detect and track various types of objects from your phones camera such as lines, colour blobs, circles, rectangles and people. Detected object types and screen positions can then be sent to a Bluetooth receiver device such as HC-05. 

If using an appropriate micro-controller e.g. Arduino or Raspberry Pi users can analyse the detected objects for further projects. A typical example could be to attach a phone to a 2 or 4W robot kit which can then track/follow a ball or person.

Key Application Features:
1. Colour Blob Detect and Track
2. Circle Detect and Track
3. Line Detect
4. People Detect and Track
5. Rectangle detection.
6. Send detected object parameters wirelessly over Bluetooth.

Note that all image processing operations work best in good lighting conditions. If you are unable to detect objects please try changing some of the configuration settings detailed below. Also note that the tracking algorithms implemented are simplistic and hence will not work reliabily when multiple objects overlap each other.   

Setting up Bluetooth:
To connect an appropriate Bluetooth receiver click the "Connect Bluetooth" button. If there is no button displayed you will need to double or single click the preview screen to bring up the options. The list shown provides the existing paired devices for your phone. If there is no items in the list, you will need to first pair your device within the Android Bluetooth settings options. Clicking on an existing paired device will attempt a connnection to that device. If successfull you will be taken back to the camera preview screen. Any errors will be displayed if not successfull.

Configuration Settings:
There are a number of configuration settings which control how the OpenCV computer vision libraries detect objects. These can be set by first clicking the "Settings" button. If there is no button showing on the preview camera screen, users will need to double or single click the preview screen to access the settings page.

Video Scale:
This controls the image size that will be used for all image processing operations. A scale factor of 1 will use the phones default image size capped to a maximum of 1280x720. If your phones CPU is not fast it is recommended to set this value to 2. Higher scale factors will result in faster image processing operations but can potentially result in instability or no object detections.

Tracked Object Persistance:
Each tracked object is given a unique id. Tracked objects however can be missed on certain image frame updates and consequently re-appear on subsequent frames. To avoid issuing new ids, each tracked object is given a life persistance value. Increasing this configuration parameter will keep objects being tracked longer in the memory buffers.

BT Serial Tx Buffer Size:
This configuration parameter controls how many detected object items are pushed onto the internal commnunication buffer for Bluetooth communication. The internal communication buffer is sampled 100 times a second (10ms). This should be sufficient to clear the buffer. However on fast image frame updates, it may become full, resulting in high communication latency. Reducing size this will improve commuincation speed but can result in missed detected object positions.

Colour Blob Settings:
Clicking this option will turn on colour blob detection. In the main camera preview screen clicking on the camera preview screen will match all items of that touched colour. The application is currently hard set to 4 objects only.

Clicking on the "Tracking" option will turn on colour blob tracking. Detected objects will be tracked with a unique id. Note that tracking algorithms are simplistic which can result in incorrect tracking of object when objects are in close proximity. 
Note: THIS OPTION MUST BE SELECTED FOR "FILTER ON" options in circle and people to work.

Circle Settings:
These settings will turn on circle detection and tracking. If too many circles are being detected you can filter the circles based on radius. 

Filter On Colour Selection: If multiple circles are being identified and you need to track a sepcific object, an additional filter option can be used to select only circles which intersect with a particular selected colour selection. Note for this to work you must have the Colour Blob Tracking selected. Tracked objects with this option will be highlighted with yellow circles.

If you are not able to select circles at all try changing the video scale settings. 

Line Settings:
This option performs a Hough transform algorithm to determine line edges within the image frame. If there are too many lines being detected you can increase the line threshold setting.

People Settings:
This option performs a Histogram of Gradients (HOG) algorithm to detect and or track people. You can combine people detection with colour blob selection filtering. Tracked objects with this option will be highlighted with yellow rectangles. 

If you are not able to select circles at all try changing the video scale settings. 

Rectangle Settings:
This feature is still in development and currently only supports rectangle detection with no Bluetooth position communication.

Tensflow Settings:
This feature is not yet supported. The aim is to use enhanced A.I deep machine learning a wider range of object types.

Bluetooth Data Transmit Formats:

All data communication is sent as ASCII text in the following format:

<Object Type>:<ID>:<XPos>,<YPos>,<Width>,<Height><NewLine>
  
  Example Colour Blob Object: "CO:0:-40,60,0,0"
  Where ID is a number between 0 and 4 with no tracking, or any unique integer tracked ID number with tracking option. 
  The x and y positions relate to the centre of the colour blob with 0,0 being at the centre of the camera preview screen. 
   
  Example Circle Object No Tracking: "CC:0:-40,60,20,0"
  Where x,y positions give centre of circle, and width gives radius of circle. 
  In tracking mode the x,y,w,h provide the inside rectangle of the circle. 

  Example Circle Object with Filter On Colour: "FC:0:-40,60,20,0"
  Where x,y positions give centre of circle, and width gives radius of circle.
  
  Example Line Object: "LO:0:-40,60,20,200"
  Where x,y positions gives first line point, and w,h givds second line point.
  
  Example People Object No Tracking: "PO:0:-40,60,20,0"
  Where x,y positions gives top left of rectangle, and w, h gives width and height. 
  
  Example People Object with Filter On Colour: "FP:0:-40,60,20,0"
  Where x,y positions gives top left of rectangle, and w, h gives width and height of rectangle.
  
  See sample Arduino sketch for above parsing of the text data.
  
 
Copyright Gemcode Studios 2019

If you have any sample Arduino projects used with Ard Object Detection and Tracking application please get in contact. Happy tinkering. 











