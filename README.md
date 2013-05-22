#UAXMLParser – Replacement for NSXMLParser

As a result of my first product, I have created a replacement XML parser for iOS platforms. It is written in C/C++/Objective-C and does not utilize any libraries for it’s operation. Instead the XML parser is written from scratch, using the Boyer-Moore search algorithm to make it fast.

It offers some advantages over NSXMLParser

- It’s faster!
- It has the ability to see forward in data.
- It has a build-in delegate stack. See below for details.
- Has the ability to manually traverse the XML node tree.
- It’s easy to implement.

##The Delegate Stack

The delegate stack, as mentioned above, is an attempt at solving the handing of parsing XML using multiple delegates. A typical parsing scenario in Cocoa is the approach of passing the parser delegate around multiple objects. As an example, if one object encounters a node which it is not able to parse, it will pass the parser to an object capable of parsing this node. Traditionally this is done by setting the parsers delegate to the new object responsible of doing further XML parsing. This leaves a question of who is responsible of passing the parser back to the originating object.

The delegate stack is an attempt of answering that question. Every time the parser is passed to another object for further parsing, the object (the originating object or the object taking over the delegate) will push onto the delegate stack using the pushDelegate: message receiver. If this is done from within the delegates implementation of parser:DidBeginNode:, the parser will automatically pop the delegate when the parser finishes the node. This way the parser takes over the responsibility of giving back the delegate to the originating object.

Messages from the parser are only passed to the top delegate of the stack. So when a delegate has been pushed onto the stack, the previous delegate will not receive any messages until the newly pushed delegate is popped. Either automatically by the parser or manually using the popDelegate: message receiver.

Notice: Automatically popped delegates are removed from stack prior to the parser sending the parser:didEndNode: message. This means that the delegates receiving the parser:didBeginNode: will also receive the parser:didEndNode: message. The pushed delegate will receive all messages of nodes found in between.

You can disable the automatic popping of delegates by setting the automaticallyPopsDelegatesOffStack property of the UAXMLParser class to NO.

##Implementation

The code is NOT documented, but it’s simplicity makes it easy to implement. Just implement the UAXMLParserDelegate protocol in whatever class implementation that needs XML parsing support. This protocol has one required message receiver and three optional.

### parser:didBeginNode: (required)

    - (void)parser:(UAXMLParser*)parser didBeginNode:(UAXMLNode*)node

This receiver is the prime receiver of the delegate. It informs the top delegate of the stack, that a new node has been found. Contrary to NSXMLParser, the parser does not pass the node name, namespace or attributes. Instead these values are accessible from the UAXMLNode object passed by the parser.

### parser:didEndNode: (optional)

    - (void)parser:(UAXMLParser*)parser didEndNode:(UAXMLNode*)node

This receiver informs the top delegate of the stack, that the parser has finished parsing a node. Any automatically popped delegates has been popped prior to this message.

### parserWillBeginParse: (optional)

    - (BOOL)parserWillBeginParse:(UAXMLParser*)parser

This receiver informs the top delegate of the stack, that parsing of an XML file is about to begin. The delegate then has the ability to abort the operation by returning NO. The delegate could retain the parser, in order to start parsing itself when the delegate is ready.

### parserDidEndParse: (optional)

    - (void)parserDidEndParse:(UAXMLParser*)parser

Informs the top delegate of the stack, that the parser has finished parsing a document.

## Other options

The UAXMLParser class is instantiated using the initWithContentOfUrl: message receiver. This receiver takes an NSURL object as it’s initiating parameter, which is fetches and made ready for parse. It will return nil if an error occurred. As an example this could be an invalid NSURL or an invalid document was fetched.

Other than that, the UAXMLParser has the pushDelegate: and popDelegate: message receivers. It also has an ignoreChildNodes receiver, which can be called by the delegate, if no further sub-processing of the current node should take place. This allows the delegates to decide whether the time consuming process of parsing the child nodes of a specific node, should be skipped, thereby speeding up parse time. This is especially useful when parsing large XML documents, that only needs partial parsing.

Lastly the UAXMLNode class has an NSArray of the it’s child nodes and the ability to get an NSArray containing child nodes with a specific name. It is able (using the valueOfAttributeWithName: message receiver) to get the value of a specified attribute. It also has the ability to access it’s parent node (details on that below). The innerValue message receiver is used to get the text content of a node. This is instead of the parser:foundCharacters: message receiver found in NSXMLParserDelegate.

You begin the parse by sending the UAXMLParser object the parse message.

## Manual Traversal

UAXMLParser offers the documentNode property, which is essentially the UAXMLNode representing the document. This you can use to manually parse the XML node tree, instead of using the delegate implementations. UAXMLNode has the childNodes property, which returns an NSArray containing the node’s child nodes.

Furthermore it has the nodesWithTagName: message receiver, which takes a node name, and returns an NSArray containing all child nodes with the specified name.

This, along with the valueOfAttributeWithName: and innerValue message receivers, enables you to get rid of the delegate implementation, and instead do manual parsing. This is useful if you have small XML documents, where the data structure is known in advance, and can help to reduce parsing code complexity.

## Behind the Scenes

The code utilizes a kind of process-by-request approach, meaning that the XML is only being processed when needed. Attributes and nodes are being parsed when requested. Everything is cached internally to speed up parse time. If you request a node or attribute twice, the second request will be substantially faster. Memory management of internal caching is handled internally.

Be aware of the parentNode property of UAXMLNode. The UAXMLNode is a wrapper object for the C++ class XMLNode. Because the code works in a hierarchical structure, UAXMLNode is not aware of any wrapper objects for it’s parent. Therefore it creates a new wrapper object for it’s parent node. This can lead to memory consumption and UAXMLNode objects contained using the parentNode property, should ALWAYS be compared to other UAXMLNode objects using the isEqual: message receiver. Further more, any caching already done by other wrapper objects will be rebuild by the newly created object.

That said, the tradeoff is not substantial. XML parsing is fast because of the Boyer-Moore search algorithm, which also makes rebuilding of caches fast. Furthermore the newly created parent node is cached within UAXMLNode, so subsequent requests will take zero time.

Therefore if you must obtain the parent node, feel free to do so, as it will not make you code slow per se from a user perspective. It only adds a slight overhead in terms of speed and memory consumption, but it’s not dangerous in any way to do so.

All classes within the node tree uses the same internal buffer as their individual buffer. The document node is responsible of freeing up the buffer, when the parser is destroyed. This way, the memory consumption of each UAXMLNode is limited, as if operating on the same XML document, also operated on the same internal buffer, thereby only adding the overhead of the class structure. Which is to say the least, not much.

## Memory management

The code follows the traditional Core Foundation rules of memory management, meaning the code responsible for creating an object is also responsible of destroying it. Because of the bridging between C++ and Objective-C, single nodes in C++ scope might have multiple Objective-C wrappers objects (as mentioned above). Therefore do not compare UAXMLNode objects by pointer. They might be the same node, even though you could be comparing two different wrapper objects. This is a tradeoff as a result of keeping up with the Core Foundation memory management model. Instead use the isEqual: which uses an internal pointer comparison. This way you can determine if two UAXMLNode objects are the same.

Also notice: The code has not been tested in a garbage collected environment. Please feel free to comment or write me with any experiences with this issue.

## Common disadvantages with NSXMLParser

This parser has some disadvantages in common with Apple’s NSXMLParser. One is, that it still needs to download the entire XML document before parsing can begin. That means you still have the responsiveness of the data source to take into account. If this is a file system, it’s no problem. If this is a remote server, and you’re dealing with large XML documents, this could pose a problem. Parsing on the incoming data stream could be an idea for future development.

## Lastly – but not least

The parser does not support XML namespaces. Instead a namespace of a node is seen as a part of the node name. That means that a node with the namespace base and node name address will be approached as a node with the name base:address. There are pros and cons in not implementing namespaces. But the prime objective has been to keep the code lightweight and fast. If you really need a parser that support namespaces, you should consider using the NSXMLParser instead.

The code is licensed under the two clause BSD-license, which basically means you can do anything but sue me! For more information see the LICENSE file.

## Considerations

There are some considerations to take into account. Use this parser, if you need a more flexible parser than NSXMLParser, but still needs to adapt it into existing code. If you are absolutely dependent on XML namespaces, do not use this code. But if you need a parser, that is able to do more, than just tell your delegates what it finds, when it finds it, this parser might be an alternative. If you need a parser, that acts similar to NSXMLParser, but speed is an issue, this parser is also an option.

Lastly feel free to modify, improve or rewrite the code. I would love to get some feedback!

That’s all folks!

**Hope you will enjoy.**
