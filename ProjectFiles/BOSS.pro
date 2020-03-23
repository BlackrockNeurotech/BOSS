#-------------------------------------------------
#
# Project created by QtCreator 2014-05-13T14:29:08
#
#-------------------------------------------------

QT += core gui network
QT += opengl widgets
QT += concurrent
CONFIG += c++11



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BOSS
TEMPLATE = app

HEADERS += ../backend/file/NexFile.h \
    ../libs/DspFilters/Bessel.h \
    ../libs/DspFilters/Biquad.h \
    ../libs/DspFilters/Butterworth.h \
    ../libs/DspFilters/Cascade.h \
    ../libs/DspFilters/ChebyshevI.h \
    ../libs/DspFilters/ChebyshevII.h \
    ../libs/DspFilters/Common.h \
    ../libs/DspFilters/Custom.h \
    ../libs/DspFilters/Design.h \
    ../libs/DspFilters/Dsp.h \
    ../libs/DspFilters/Elliptic.h \
    ../libs/DspFilters/Filter.h \
    ../libs/DspFilters/Layout.h \
    ../libs/DspFilters/Legendre.h \
    ../libs/DspFilters/MathSupplement.h \
    ../libs/DspFilters/Params.h \
    ../libs/DspFilters/PoleFilter.h \
    ../libs/DspFilters/RBJ.h \
    ../libs/DspFilters/RootFinder.h \
    ../libs/DspFilters/SmoothedFilter.h \
    ../libs/DspFilters/State.h \
    ../libs/DspFilters/Types.h \
    ../libs/DspFilters/Utilities.h \
    ../model/clusterdata.h \
    ./AppVersion.h \
    ./licenseCheck.h \
    ../backend/dataprocessing/spike/ManualSorting.h \
    ../backend/dataprocessing/spike/templates.h \
    ../backend/file/fileutilities.h \
    ../graphics/items/timewaveformgraphicsitems.h \
    ../graphics/items/waveformgraphicsitems.h \
    ../model/EditAction.h \
    ../model/EditLines.h \
    ../model/spikedata.h \
    ./dockMap.h \
    ../backend/dataprocessing/continuous/FFT.h \
    ../graphics/items/FFTgraphicsitem.h \
    ../backend/dataprocessing/common/Utils.h \
    ../backend/dataprocessing/common/accumulator.h \
    ../graphics/items/alignedtextitem.h \
    ../graphics/items/alignedtextslideritem.h \
    ../backend/file/btree.h \
    ../backend/file/cdfile.h \
    ../backend/file/channelfilebase.h \
    ../backend/dataprocessing/spike/clusteringtendencyccumulator.h \
    ../graphics/items/cursorshapegraphicsitem.h \
    ../backend/dataprocessing/continuous/dspfilter.h \
    ../backend/dataprocessing/continuous/energyfilter.h \
    ../backend/file/fileformatbase.h \
    ../backend/dataprocessing/continuous/histograms.h \
    ../graphics/items/horizontalunitsgraphicsitem.h \
    ../backend/dataprocessing/spike/kmeans.h \
    ../graphics/items/mouseovergraphicsitem.h \
    ../backend/file/nevfile.h \
    ../backend/file/nsxfile.h \
    ../backend/dataprocessing/spike/pca.h \
    ../backend/file/sdfile.h \
    ../backend/dataprocessing/spike/sorting.h \
    ../backend/dataprocessing/spike/spikeaccumulators.h \
    ../backend/dataprocessing/spike/tdistributionem.h \
    ../backend/dataprocessing/continuous/timescanaccumulator.h \
    ../backend/dataprocessing/continuous/timesliceaccumulator.h \
    ../graphics/items/tooltipgraphicsitem.h \
    ../graphics/items/unitgraphicsitem.h \
    ../backend/dataprocessing/spike/waveformalignment.h \
    ../backend/network/readURL.h \
    ../widgets/loadparametersdialog.h \
    ../widgets/preferencesdialog.h \
    ../widgets/MainWidgetToolBar.h \
    ../widgets/RecentFileItem.h \
    ../graphics/correlogramgraphicsview.h \
    ../widgets/optionsslider.h \
    ../widgets/correlograms.h \
    ./mysplasscreen.h \
    ../backend/file/fileextractor.h \
    ../backend/dataprocessing/common/unitassignment.h \
    ../widgets/verticallabel.h \
    ../model/layoutmodel.h \
    ../model/WindowData.h \
    ../model/layoutitem.h \
    ../widgets/DockWidgetTitleBar.h \
    ../widgets/DockWidget.h \
    ../graphics/items/segmentgraphicsitem.h \
    ../graphics/items/endpointgraphicsitem.h \
    ../model/mapperwidgets/spinBoxGroup.h \
    ../model/Proxy/channeltableproxymodel.h \
    ../widgets/uniteditmenu.h \
    ./dockLayout.h \
    ../widgets/zoomtoolbar.h \
    ../widgets/MenuItem.h \
    ../widgets/expandbutton.h \
    ../widgets/optionsdetectframe.h \
    ../widgets/optionssortframe.h \
    ../model/ChannelData.h \
    ../widgets/ModeSlider.h \
    ../widgets/ModeWidgetAction.h \
    ../widgets/PointSizeSlider.h \
    ../widgets/PointSizeWidgetAction.h \
    ../model/ProcessQ.h \
    ../widgets/ScrollRangeToolBar.h \
    ../widgets/UnitSelectionAction.h \
    ../widgets/UnitsSplitter.h \
    ../widgets/alignmenttoolbar.h \
    ../widgets/animatedbutton.h \
    ../widgets/arrowbutton.h \
    ../graphics/basegraphicsview.h \
    ../model/channelItem.h \
    ../model/channelconfigmodel.h \
    ../widgets/channeltableview.h \
    ../model/mapperwidgets/checkboxgroup.h \
    ../model/mapperwidgets/checkradiogroup.h \
    ../model/configmodel.h \
    ../backend/dataprocessing/continuous/continuousbase.h \
    ../graphics/items/continuousgraphicsitem.h \
    ../backend/file/datasaver.h \
    ../backend/file/diskaccesscontroller.h \
    ../model/mapperwidgets/editgroup.h \
    ../widgets/editmenu.h \
    ../graphics/energygraphicsview.h \
    ../widgets/feature2dtoolbar.h \
    ../widgets/feature3dtoolbar.h \
    ../widgets/featurecorrelation.h \
    ../backend/dataprocessing/spike/featureextraction.h \
    ../graphics/items/featuregraphicsitem.h \
    ../graphics/featuregraphicsview.h \
    ../widgets/featurevsfeature.h \
    ../graphics/FFTGraphicsView.h \
    ../backend/file/fileinterface.h \
    ../widgets/glwidget.h \
    ../graphics/items/histogramgraphicsitem.h \
    ../graphics/items/horizontalgraphicsitem.h \
    ../model/intervalData.h \
    ./mainwindow.h \
    ../widgets/multipleChoiceSlider.h \
    ../graphics/peakheightsgraphicsview.h \
    ../model/mapperwidgets/radiogroup.h \
    ../graphics/items/scrollbargraphicsitem.h \
    ../graphics/scrollrangegraphicsview.h \
    ../graphics/items/signalenergygraphicsitem.h \
    ../graphics/signalenergygraphicsview.h \
    ../backend/dataprocessing/spike/spikeproxymodel.h \
    ../backend/dataprocessing/spike/spikescan.h \
    ../backend/dataprocessing/spike/spikeslice.h \
    ../graphics/timegraphicsview.h \
    ../backend/dataprocessing/continuous/timeproxymodel.h \
    ../backend/dataprocessing/continuous/timescan.h \
    ../backend/dataprocessing/continuous/timeslice.h \
    ../graphics/timewaveformgraphicsview.h \
    ../widgets/toolbarWidgetAction.h \
    ../widgets/unitSelection.h \
    ../graphics/unitgraphicsview.h \
    ../graphics/items/verticalgraphicsitem.h \
    ../graphics/waveformgraphicsview.h \
    ../backend/file/worker.h \
    ../widgets/DockToolbar.h \
    ../backend/file/CsvFile.h \
    ../backend/file/DataSaverIO.h \
    ../backend/file/MatFile.h \
    ../widgets/BlackrockButton.h \
    ../widgets/CheckBoxBlank.h \
    ../widgets/ComboWidgetAction.h \
    ../widgets/DoubleSpinBoxBlank.h \
    ../widgets/SpinBoxBlank.h \
    ../widgets/SpinWidgetAction.h \
    welcomepage.h \
    mergefilesdialog.h \
    savemergedfilesdialog.h \
    ../model/BatchQ.h \
    ../graphics/items/separatorsgraphicsitem.h

SOURCES += ../backend/dataprocessing/common/unitassignment.cpp \
    ../backend/dataprocessing/spike/ManualSorting.cpp \
    ../backend/dataprocessing/spike/templates.cpp \
    ../backend/file/fileextractor.cpp \
    ../backend/file/fileutilities.cpp \
    ../backend/network/readURL.cpp \
    ../backend/dataprocessing/continuous/FFT.cpp \
    ../graphics/correlogramgraphicsview.cpp \
    ../graphics/items/endpointgraphicsitem.cpp \
    ../graphics/items/FFTgraphicsitem.cpp \
    ../graphics/items/segmentgraphicsitem.cpp \
    ../graphics/items/timewaveformgraphicsitems.cpp \
    ../graphics/items/waveformgraphicsitems.cpp \
    ../libs/DspFilters/Bessel.cpp \
    ../libs/DspFilters/Biquad.cpp \
    ../libs/DspFilters/Butterworth.cpp \
    ../libs/DspFilters/Cascade.cpp \
    ../libs/DspFilters/ChebyshevI.cpp \
    ../libs/DspFilters/ChebyshevII.cpp \
    ../libs/DspFilters/Custom.cpp \
    ../libs/DspFilters/Design.cpp \
    ../libs/DspFilters/Documentation.cpp \
    ../libs/DspFilters/Elliptic.cpp \
    ../libs/DspFilters/Filter.cpp \
    ../libs/DspFilters/Legendre.cpp \
    ../libs/DspFilters/Param.cpp \
    ../libs/DspFilters/PoleFilter.cpp \
    ../libs/DspFilters/RBJ.cpp \
    ../libs/DspFilters/RootFinder.cpp \
    ../libs/DspFilters/State.cpp \
    ../model/clusterdata.cpp \
    ../model/EditAction.cpp \
    ../model/EditLines.cpp \
    ../model/layoutitem.cpp \
    ../model/layoutmodel.cpp \
    ../model/mapperwidgets/spinBoxGroup.cpp \
    ../model/Proxy/channeltableproxymodel.cpp \
    ../model/spikedata.cpp \
    ../model/WindowData.cpp \
    ../widgets/correlograms.cpp \
    ../widgets/DockWidget.cpp \
    ../widgets/DockWidgetTitleBar.cpp \
    ../widgets/expandbutton.cpp \
    ../widgets/loadparametersdialog.cpp \
    ../widgets/MainWidgetToolbar.cpp \
    ../widgets/MenuItem.cpp \
    ../widgets/ModeSlider.cpp \
    ../widgets/ModeWidgetAction.cpp \
    ../widgets/optionsslider.cpp \
    ../widgets/PointSizeSlider.cpp \
    ../widgets/PointSizeWidgetAction.cpp \
    ../model/ProcessQ.cpp \
    ../widgets/preferencesdialog.cpp \
    ../widgets/RecentFileItem.cpp \
    ../widgets/uniteditmenu.cpp \
    ../widgets/UnitSelectionAction.cpp \
    ../widgets/UnitsSplitter.cpp \
    ../backend/dataprocessing/common/Utils.cpp \
    ../backend/dataprocessing/common/accumulator.cpp \
    ../graphics/items/alignedtextitem.cpp \
    ../graphics/items/alignedtextslideritem.cpp \
    ../widgets/alignmenttoolbar.cpp \
    ../widgets/animatedbutton.cpp \
    ../widgets/arrowbutton.cpp \
    ../graphics/basegraphicsview.cpp \
    ../backend/file/btree.cpp \
    ../backend/file/cdfile.cpp \
    ../model/channelData.cpp \
    ../model/channelItem.cpp \
    ../model/channelconfigmodel.cpp \
    ../backend/file/channelfilebase.cpp \
    ../widgets/channeltableview.cpp \
    ../model/mapperwidgets/checkboxgroup.cpp \
    ../model/mapperwidgets/checkradiogroup.cpp \
    ../model/configmodel.cpp \
    ../backend/dataprocessing/continuous/continuousbase.cpp \
    ../graphics/items/continuousgraphicsitem.cpp \
    ../graphics/items/cursorshapegraphicsitem.cpp \
    ../backend/file/datasaver.cpp \
    ../backend/file/diskaccesscontroller.cpp \
    ../backend/dataprocessing/continuous/dspfilter.cpp \
    ../model/mapperwidgets/editgroup.cpp \
    ../widgets/editmenu.cpp \
    ../backend/dataprocessing/continuous/energyfilter.cpp \
    ../graphics/energygraphicsview.cpp \
    ../widgets/feature2dtoolbar.cpp \
    ../widgets/feature3dtoolbar.cpp \
    ../widgets/featurecorrelation.cpp \
    ../backend/dataprocessing/spike/featureextraction.cpp \
    ../graphics/items/featuregraphicsitem.cpp \
    ../graphics/featuregraphicsview.cpp \
    ../widgets/featurevsfeature.cpp \
    ../graphics/fftgraphicsview.cpp \
    ../backend/file/fileformatbase.cpp \
    ../backend/file/fileinterface.cpp \
    ../widgets/glwidget.cpp \
    ../graphics/items/histogramgraphicsitem.cpp \
    ../backend/dataprocessing/continuous/histograms.cpp \
    ../graphics/items/horizontalgraphicsitem.cpp \
    ../graphics/items/horizontalunitsgraphicsitem.cpp \
    ../model/intervalData.cpp \
    ../backend/dataprocessing/spike/kmeans.cpp \
    ./main.cpp \
    ./mainwindow.cpp \
    ../graphics/items/mouseovergraphicsitem.cpp \
    ../widgets/multipleChoiceSlider.cpp \
    ../backend/file/nevfile.cpp \
    ../backend/file/nsxfile.cpp \
    ../backend/dataprocessing/spike/pca.cpp \
    ../graphics/peakheightsgraphicsview.cpp \
    ../model/mapperwidgets/radiogroup.cpp \
    ../graphics/items/scrollbargraphicsitem.cpp \
    ../graphics/scrollrangegraphicsview.cpp \
    ../widgets/scrollrangetoolbar.cpp \
    ../backend/file/sdfile.cpp \
    ../graphics/items/signalenergygraphicsitem.cpp \
    ../graphics/signalenergygraphicsview.cpp \
    ../backend/dataprocessing/spike/sorting.cpp \
    ../backend/dataprocessing/spike/spikeaccumulators.cpp \
    ../backend/dataprocessing/spike/spikeproxymodel.cpp \
    ../backend/dataprocessing/spike/spikescan.cpp \
    ../backend/dataprocessing/spike/spikeslice.cpp \
    ../backend/dataprocessing/spike/tdistributionem.cpp \
    ../graphics/timegraphicsview.cpp \
    ../backend/dataprocessing/continuous/timeproxymodel.cpp \
    ../backend/dataprocessing/continuous/timescan.cpp \
    ../backend/dataprocessing/continuous/timescanaccumulator.cpp \
    ../backend/dataprocessing/continuous/timeslice.cpp \
    ../backend/dataprocessing/continuous/timesliceaccumulator.cpp \
    ../graphics/timewaveformgraphicsview.cpp \
    ../widgets/toolbarWidgetAction.cpp \
    ../graphics/items/tooltipgraphicsitem.cpp \
    ../widgets/unitSelection.cpp \
    ../graphics/items/unitgraphicsitem.cpp \
    ../graphics/unitgraphicsview.cpp \
    ../graphics/items/verticalgraphicsitem.cpp \
    ../backend/dataprocessing/spike/waveformalignment.cpp \
    ../graphics/waveformgraphicsview.cpp \
    ../widgets/zoomtoolbar.cpp \
    ./licenseCheck.cpp \
    ./mysplasscreen.cpp \
    ../widgets/verticallabel.cpp \
    ./dockLayout.cpp \
    ./dockMap.cpp \
    ../widgets/DockToolbar.cpp \
    ../widgets/optionsdetectframe.cpp \
    ../widgets/optionssortframe.cpp \
    ../backend/file/CsvFile.cpp \
    ../backend/file/DataSaverIO.cpp \
    ../backend/file/MatFile.cpp \
    ../backend/file/NexFile.cpp \
    ../widgets/BlackrockButton.cpp \
    ../widgets/CheckBoxBlank.cpp \
    ../widgets/ComboWidgetAction.cpp \
    ../widgets/DoubleSpinBoxBlank.cpp \
    ../widgets/SpinBoxBlank.cpp \
    ../widgets/SpinWidgetAction.cpp \
    welcomepage.cpp \
    mergefilesdialog.cpp \
    savemergedfilesdialog.cpp \
    ../model/BatchQ.cpp \
    ../backend/file/worker.cpp \
    ../graphics/items/separatorsgraphicsitem.cpp

FORMS += ./mainwindow.ui \
    ../widgets/optionsslider.ui \
    ../widgets/optionsdetectframe.ui \
    ../widgets/optionssortframe.ui \
    ../widgets/loadParametersDialog.ui \
    ../widgets/preferencesdialog.ui \
    welcomepage.ui \
    mergefilesdialog.ui \
    savemergedfilesdialog.ui

RESOURCES += OFSResources.qrc

ICON = BOSS.icns

unix:!mac {
## Armadillo
INCLUDEPATH += ../libs/armadillo/include

##CodeMeter
INCLUDEPATH += ../libs/codemeter

#libmatio
INCLUDEPATH += /usr/local/include

LIBS += -lblas -llapack
LIBS += -lz
LIBS += -L/usr/local/lib -lmatio
LIBS += -L/usr/local/hdf5/lib -lhdf5
LIBS += -lwibucmlin64

DEFINES += OSLINUX
}

macx: {
QMAKE_MAC_SDK = macosx10.11
## Boost
INCLUDEPATH += /usr/local/boost_1_54_0

## Armadillo
INCLUDEPATH += /usr/local/armadillo/include

##CodeMeter
INCLUDEPATH += ../libs/codemeter

#libmatio
INCLUDEPATH += ../libs/matlab/mac/include

LIBS += -framework Accelerate
LIBS += -lz
LIBS += -L$$PWD/../libs/matlab/mac/lib -lhdf5 -lmatio

DEFINES += OSMAC


LIBS += -F$$PWD/../../../../../Library/Frameworks/ -framework WibuCmMacX
INCLUDEPATH += $$PWD/../../../../../Library/Frameworks
DEPENDPATH += $$PWD/../../../../../Library/Frameworks
}



