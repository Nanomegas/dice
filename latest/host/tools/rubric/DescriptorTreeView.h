///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __DESCRIPTORTREEVIEW_H__
#define __DESCRIPTORTREEVIEW_H__


//==============================================================================
class DescriptorTreeView  : public Component,
                            public ButtonListener,
                            public TextEditorListener,
                            public ActionBroadcaster,
                            public ActionListener,
                            public DragAndDropContainer
{
    XmlElement*         treeXml;
    XmlElement*         infoBlocksXml;

    TreeViewItem*       rootItem;
    TreeView*           treeView;
    TextEditor*         hexText;
    TextEditor*         notesText;
    TextButton*         refreshButton;
    TextButton*         commentsButton;
    TextButton*         bytesButton;
    Label*              commentsLabel;
    
    bool                bAddComments;
    bool                bHexStream;
    MemoryOutputStream* outStream;
    
public:
    friend class DescriptorTreeViewItem;
    //==============================================================================
    DescriptorTreeView();
    DescriptorTreeView(File* DescriptorFile);
    ~DescriptorTreeView();

    //==============================================================================
    void actionListenerCallback(const String &msg);
    void paint (Graphics& g);
    void buttonClicked (Button* button);
    void resized();
    void textEditorTextChanged(TextEditor & editor);
    void textEditorEscapeKeyPressed(TextEditor & editor);
    void textEditorReturnKeyPressed(TextEditor & editor);
    void textEditorFocusLost(TextEditor & editor);
    void UpdateTree();
    void CondenseTree();
    void ExpandTree();
    bool saveXmlTree(const String& fileName);
    void scrubDeletedIbs(XmlElement* parentElement);
    int processDescriptorTree();
    
    int GetMusicSUStatusAreaIBLength(XmlElement * xmlInfoBlock);
    int GetRoutingStatusIBLength(XmlElement * xmlInfoBlock);
//    int GetSrcPlugStatusIBLength(XmlElement * xmlInfoBlock);
//    int GetAudioIBLength(XmlElement * xmlInfoBlock);
//    int GetMidiIBLength(XmlElement * xmlInfoBlock);
//    int GetSyncIBLength(XmlElement * xmlInfoBlock);
//    int GetMusicOutputPlugStatusAreaIBLength(XmlElement * xmlInfoBlock);
    int GetSUPlugIBLength(XmlElement * xmlInfoBlock);
    int GetClusterIBLength(XmlElement * xmlInfoBlock);
    int GetMusicPlugIBLength(XmlElement * xmlInfoBlock);
    int GetNameIBLength(XmlElement * xmlInfoBlock);

    void writeMusicSUStatusAreaIB(XmlElement * xmlInfoBlock);
    void writeRoutingStatusIB(XmlElement * xmlInfoBlock);
//    void writeSrcPlugStatusIB(XmlElement * xmlInfoBlock);
//    void writeAudioIB(XmlElement * xmlInfoBlock);
//    void writeMidiIB(XmlElement * xmlInfoBlock);
//    void writeSyncIB(XmlElement * xmlInfoBlock);
//    void writeMusicOutputPlugStatusAreaIB(XmlElement * xmlInfoBlock);
    void writeSUPlugIB(XmlElement * xmlInfoBlock);
    void writeClusterIB(XmlElement * xmlInfoBlock);
    void writeSignalIB(XmlElement * signalIB);
    void writeMusicPlugIB(XmlElement * xmlInfoBlock);
    void writeNameIB(XmlElement * xmlInfoBlock);
    
    juce_UseDebuggingNewOperator

};
#endif   // __DESCRIPTORTREEVIEW_H__
