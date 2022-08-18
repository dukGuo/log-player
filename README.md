# log-player

A simple tool for log-analysis.

[slides](slides/slides.html)

# Installation

Compiled with Qt 5.15.2

[Qt 5.15 ](https://download.qt.io/official_releases/qt/5.15/5.15.2/)

# Functions

* [X] View Local Log-file
* [X] DisPlay Line Number in Viewer
* [X] Jump To Specific Line
* [X] High-Light Keyword in Viewer
* [X] Search Keyword in Log
* [X] Filter Needless Log with Specific pattern
* [X] Generate Timeline and Export as Picture
* [X] Split Large File
* [X] Merge Small Files

# Bug Lists

* [X] When clicked "shortcut", the application will crash.

| **Cause**    | controller.cpp:  in getShortcut(), visit a Null pointer |
| ------------------ | ------------------------------------------------------- |
| **Solution** | **Check before visit**                            |

* [X] Scroll bar was hidden and no reaction for click in LogVeiw

| Cause              | logviewer.cpp:  in buiding function, function return earlier than excpected,<br />which cause Scoll bar can be binded when function return. |
| ------------------ | -------------------------------------------------------------------------------------------------------------------------------------------- |
| **Solution** | **In each exits, add Scoll bar's binding**                                                                                             |

* [X] Sub log always be hidden

| Cause              | forget to delete test block,which makes tab-> count always larger than 1.<br />So tab->show() will never be reached |
| ------------------ | ------------------------------------------------------------------------------------------------------------------- |
| **Solution** | **elete the test block**                                                                                      |

* [X] If choose/change the same color for one tag, taglist will add this tag repeatedly. If one of them were removed , the tag's highlight will disappear.

| Cause              | when emitting colorChange signal, funtion dosen't Check the difference<br /> between old and selected color |
| ------------------ | ----------------------------------------------------------------------------------------------------------- |
| **Solution** | **Check difference between old and selected color ahead of emitting signal**                          |

* [X] In highlight/find/filter dialog widget, if closed without any input,  the dialog will cause the application crash.

| Cause              | No handle for reject in dialog,                                                  |
| ------------------ | -------------------------------------------------------------------------------- |
| **Solution** | **if input is empty and  dialog is rejected, just do nothing but return** |

* [X] Twice repeated pop-ups when clicked menu.

| Cause              | Repeatedly connect signals and slots   |
| ------------------ | -------------------------------------- |
| **Solution** | **Delete redundant connections** |
