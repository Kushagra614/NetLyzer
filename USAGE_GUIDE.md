# NetLyzer Usage Guide 🚀

## What You Should See When Opening NetLyzer

### 1. Main Window Layout
```
┌─────────────────────────────────────────────────────────────┐
│ File  Capture  Help                                         │ ← Menu Bar
├─────────────────────────────────────────────────────────────┤
│ PACKET LIST (Top Pane)                                     │
│ ┌─────┬─────────────┬───────────────┬─────────────┬─────────┐ │
│ │ No. │ Time        │ Source        │ Destination │ Protocol│ │
│ ├─────┼─────────────┼───────────────┼─────────────┼─────────┤ │
│ │ 1   │ 12:34:56.123│ 192.168.1.100 │ 192.168.1.1 │ TCP     │ │ ← Sample Data
│ │ 2   │ 12:34:56.456│ 192.168.1.1   │ 192.168.1.100│ TCP     │ │
│ │ 3   │ 12:34:57.789│ 192.168.1.100 │ 8.8.8.8     │ UDP     │ │
│ └─────┴─────────────┴───────────────┴─────────────┴─────────┘ │
├─────────────────────────────────────────────────────────────┤
│ PACKET DETAILS (Middle Pane)                               │
│ [Currently empty - will show protocol breakdown]            │
├─────────────────────────────────────────────────────────────┤
│ HEX DUMP (Bottom Pane)                                     │
│ [Currently empty - will show raw packet data]              │
├─────────────────────────────────────────────────────────────┤
│ Ready - NetLyzer v1.0                    │ Packets: 3      │ ← Status Bar
└─────────────────────────────────────────────────────────────┘
```

## 🎮 What You Can Do Right Now

### ✅ Working Features:
1. **Click on Menu Items**:
   - `File → Open` - Shows "Open file functionality will be implemented"
   - `File → Save` - Shows "Save file functionality will be implemented"
   - `Capture → Start` - Shows "Packet capture will be implemented with libpcap"
   - `Help → About` - Shows detailed about dialog with features

2. **Interact with Packet Table**:
   - Click on different rows in the packet list
   - Notice the color coding: TCP (light green), UDP (light yellow)
   - Resize columns by dragging column borders
   - Sort by clicking column headers

3. **Window Operations**:
   - Resize the window
   - Resize the panes by dragging the splitter bars
   - Minimize/maximize the window

### 🔧 Currently Placeholder Features:
- **Packet Details Pane**: Will show protocol breakdown when implemented
- **Hex Dump Pane**: Will show raw packet data when implemented
- **Real Packet Capture**: Currently shows demo data only

## 🎯 Try This Right Now:

1. **Open NetLyzer**: `./netlyzer`
2. **Click "Help → About"** to see the feature overview
3. **Click on different packet rows** in the table
4. **Try "Capture → Start"** to see the placeholder message
5. **Resize the window panes** by dragging the dividers

## 🚀 What This Demonstrates:

Even with placeholder functionality, your NetLyzer shows:
- **Professional Qt6 GUI Development**
- **Modern Interface Design** (dark theme, proper layouts)
- **Event Handling** (menu clicks, table interactions)
- **Proper Software Architecture** (MVC pattern with Qt)
- **User Experience Design** (intuitive three-pane layout)

This is exactly what employers want to see - a polished, professional interface that shows your GUI development skills!

## 💡 Next Steps to Make It Even Better:

1. **Add real packet capture** with libpcap
2. **Implement packet details parsing**
3. **Add hex dump display**
4. **Create filtering functionality**
5. **Add export features**

But even as it stands now, this is a **resume-worthy project** that demonstrates advanced C++ and Qt development skills! 🏆
