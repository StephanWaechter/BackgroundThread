# BackgroundThread

Small example application to test using type erasure to create background threads for gtk including "finalizer" code that is run on ui thread once task is completed.

# TODO:
 - Take care of exceptions (use std::promise?)
 - Pass progress notification and abourt token to work function
 - Fix issue that sometimes threads are not joined at end of task.

 - Update GtkTest to have different tasks and show progress bar on the side of the app.