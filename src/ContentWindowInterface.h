/*********************************************************************/
/* Copyright (c) 2011 - 2012, The University of Texas at Austin.     */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#ifndef CONTENT_WINDOW_INTERFACE_H
#define CONTENT_WINDOW_INTERFACE_H

#define HIGHLIGHT_TIMEOUT_MILLISECONDS 1500
#define HIGHLIGHT_BLINK_INTERVAL 250 // milliseconds

#include <QtGui>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class ContentWindowManager;

class ContentWindowInterface : public QObject {
    Q_OBJECT

    public:

        enum WindowState {
            UNSELECTED,   // the window is not selected and interaction changes its position/size
            SELECTED,     // the window is selected and interaction modifies its zoom/pan
            INTERACTION   // interaction within the window may be forwarded to the content source
        };

        // the state of interaction within the window (mouse emulation)
        struct InteractionState {
            double mouseX_, mouseY_;
            bool mouseLeft_, mouseRight_, mouseMiddle_;

            InteractionState()
            {
                mouseX_ = mouseY_ = 0.;
                mouseLeft_ = mouseRight_ = mouseMiddle_ = false;
            }

            template <typename Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & mouseX_;
                ar & mouseY_;
                ar & mouseLeft_;
                ar & mouseRight_;
                ar & mouseMiddle_;
            }
        };

        ContentWindowInterface() { }
        ContentWindowInterface(boost::shared_ptr<ContentWindowManager> contentWindowManager);

        boost::shared_ptr<ContentWindowManager> getContentWindowManager();

        void getContentDimensions(int &contentWidth, int &contentHeight);
        void getCoordinates(double &x, double &y, double &w, double &h);
        void getPosition(double &x, double &y);
        void getSize(double &w, double &h);
        void getCenter(double &centerX, double &centerY);
        double getZoom();
        bool getHighlighted();
        ContentWindowInterface::WindowState getWindowState();
        ContentWindowInterface::InteractionState getInteractionState();

        // button dimensions
        void getButtonDimensions(float &width, float &height);

        // aspect ratio correction
        void fixAspectRatio(ContentWindowInterface * source=NULL);

    public slots:

        // these methods set the local copies of the state variables if source != this
        // they will emit signals if source == NULL or if this is a ContentWindowManager object
        // the source argument should not be provided by users -- only by these functions
        virtual void setContentDimensions(int contentWidth, int contentHeight, ContentWindowInterface * source=NULL);
        virtual void setCoordinates(double x, double y, double w, double h, ContentWindowInterface * source=NULL);
        virtual void setPosition(double x, double y, ContentWindowInterface * source=NULL);
        virtual void setSize(double w, double h, ContentWindowInterface * source=NULL);
        virtual void scaleSize(double factor, ContentWindowInterface * source=NULL);
        virtual void setCenter(double centerX, double centerY, ContentWindowInterface * source=NULL);
        virtual void setZoom(double zoom, ContentWindowInterface * source=NULL);
        virtual void highlight(ContentWindowInterface * source=NULL);
        virtual void setWindowState(ContentWindowInterface::WindowState windowState, ContentWindowInterface * source=NULL);
        virtual void setInteractionState(ContentWindowInterface::InteractionState interactionState, ContentWindowInterface * source=NULL);
        virtual void moveToFront(ContentWindowInterface * source=NULL);
        virtual void close(ContentWindowInterface * source=NULL);

    signals:

        // emitting these signals will trigger updates on the corresponding ContentWindowManager
        // as well as all other ContentWindowInterfaces to that ContentWindowManager
        void contentDimensionsChanged(int contentWidth, int contentHeight, ContentWindowInterface * source);
        void coordinatesChanged(double x, double y, double w, double h, ContentWindowInterface * source);
        void positionChanged(double x, double y, ContentWindowInterface * source);
        void sizeChanged(double w, double h, ContentWindowInterface * source);
        void centerChanged(double centerX, double centerY, ContentWindowInterface * source);
        void zoomChanged(double zoom, ContentWindowInterface * source);
        void highlighted(ContentWindowInterface * source);
        void windowStateChanged(ContentWindowInterface::WindowState windowState, ContentWindowInterface * source);
        void interactionStateChanged(ContentWindowInterface::InteractionState interactionState, ContentWindowInterface * source);
        void movedToFront(ContentWindowInterface * source);
        void closed(ContentWindowInterface * source);

    protected:

        // optional: reference to ContentWindowManager for non-ContentWindowManager objects
        boost::weak_ptr<ContentWindowManager> contentWindowManager_;

        // content dimensions
        int contentWidth_;
        int contentHeight_;

        // window coordinates
        double x_;
        double y_;
        double w_;
        double h_;

        // panning and zooming
        double centerX_;
        double centerY_;

        double zoom_;

        // window state
        ContentWindowInterface::WindowState windowState_;

        // interaction state
        ContentWindowInterface::InteractionState interactionState_;

        // highlighted timestamp
        boost::posix_time::ptime highlightedTimestamp_;
};

#endif
