#include <iostream>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMap>

#include <QDebug>

struct Node {
    double lat;
    double lon;
};

struct Way {
    QString highwayType;
    QVector<qint64> nodes;
};

static QMap<qint64, Node> nodes;
static QMap<qint64, Way> ways;

static bool hasAllAttributes(QXmlStreamAttributes &attrs, const QStringList &names) {
    foreach(QString name, names) {
        if(! attrs.hasAttribute(name))
            return false;
    }
    return true;
}

// Read and process <node id=".." lat=".." lon=".." />
static void readNode(QXmlStreamReader &reader) {
    QXmlStreamAttributes attrs = reader.attributes();
    if(! hasAllAttributes(attrs, {"lat", "lon", "id"}))
        return;

    Node node;
    node.lat = attrs.value("lat").toDouble();
    node.lon = attrs.value("lon").toDouble();
    nodes[attrs.value("id").toLongLong()] = node;

    reader.skipCurrentElement();
}

// Read and process highways. Example:
// <way id="..">
//     <nd ref=".." />
//     <nd ref=".." />
//     <tag k="highway" v="..">
// </way>
static void readWay(QXmlStreamReader &reader) {
    QXmlStreamAttributes attrs = reader.attributes();
    if(! hasAllAttributes(attrs, {"id"}))
        return;

    Way way;

    bool isDriveableByCar = false;

    while(reader.readNextStartElement()) {
        QXmlStreamAttributes attrs = reader.attributes();

        // ensure the way is a highway
        if(reader.name() == "tag" && attrs.value("k").toString() == "highway") {
            QString highwayType = attrs.value("v").toString();

            // ensure cars can actually drive here
            if(highwayType != "footway"
                    && highwayType != "cycleway"
                    && highwayType != "bridleway"
                    && highwayType != "path")
            {
                isDriveableByCar = true;
                way.highwayType = highwayType;
            }
        } else if(reader.name() == "nd") {
            qint64 nodeId = reader.attributes().value("ref").toLongLong();
            way.nodes.append(nodeId);
        }

        reader.skipCurrentElement();
    }

    if(! isDriveableByCar)
        return;

    ways[attrs.value("id").toLongLong()] = way;
}

static void readMap(QXmlStreamReader &reader) {

    // ensure that the file starts with <osm ...> and enter this tag
    if(! reader.readNextStartElement() || reader.name() != "osm") {
        reader.raiseError("Invalid file");
    }

    // read every tag inside <osm ...>, and look for <node> and <way> tags
    while(reader.readNextStartElement()) {

        if(reader.name() == "node") {
            readNode(reader);
        } else if(reader.name() == "way") {
            readWay(reader);
        } else {
            reader.skipCurrentElement();
        }
    }
}

static void removeNodesNotPresentInWays() {
    QSet<qint64> nodesInAHighway;

    for(const Way &way : ways) {
        for(qint64 nodeId : way.nodes) {
            nodesInAHighway.insert(nodeId);
        }
    }

    // filter nodes so that only ones present in nodesInAHighway stay
    for (auto it = nodes.begin(); it != nodes.end(); ) {
        qint64 nodeId = it.key();
        if(nodesInAHighway.contains(nodeId)) {
            ++it;
        } else {
            it = nodes.erase(it);
        }
    }
}

static void addNodes() {

}

static void outputData(QFile &file) {
    QTextStream out(&file);

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<osm version=\"0.6\" generator=\"hacts-createmap\">\n";

    for(auto key : nodes.keys()) {
        const Node &node = nodes[key];
        out << "  <node id=\"" << key << "\" lat=\"" << node.lat << "\" lon=\"" << node.lon << "\" version=\"1\" />\n";
    }

    for(auto key : ways.keys()) {
        const Way &way = ways[key];
        out << "  <way id=\"" << key << "\" version=\"1\">\n";
        out << "    <tag k=\"highway\" v=\"" << way.highwayType << "\" />\n";
        for(auto node : way.nodes)
            out << "    <nd ref=\"" << node << "\" />\n";
        out << "  </way>\n";
    }

    out << "</osm>\n";
}

int main(int argc, char *argv[]) {
    QTextStream cerr(stderr);

    argc = 3;

    if(argc != 3) {
        cerr << "Usage: " << argv[0] << " <source map> <output map>\n"
            << "    This program adds lane and other information into OSM maps for\n"
            << "    transport simulation purpouses.\n";
    }

    QFile input("/home/karol/projects/hacts/hacts-createmap/potok.osm");
    if(! input.open(QIODevice::ReadOnly)) {
        cerr << "Failed to open " << argv[1] << ": " << input.errorString() << "\n";
        return 1;
    }

    QXmlStreamReader reader(&input);

    readMap(reader);
    if (reader.hasError()) {
        cerr << "Failed reading input map: " << reader.errorString();
        return 1;
    };

    removeNodesNotPresentInWays();

    addNodes();


    QFile output("/home/karol/projects/hacts/hacts-createmap/potok.output.osm");
    if(! output.open(QIODevice::WriteOnly)) {
        cerr << "Failed to open " << argv[2] << " for writing: " << output.errorString() << "\n";
        return 1;
    }
    outputData(output);

    if(output.error() != QFile::NoError) {
        cerr << "Failed writing to " << argv[2] << ": " << output.errorString() << "\n";
        return 1;
    }

    return 0;
}
