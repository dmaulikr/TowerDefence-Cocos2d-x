//
//  Grid.cpp
//  TowerDefence
//
//  Created by Hackintosh on 7/27/17.
//
//

#include <stdio.h>
#include "Grid.h"


Grid::Grid(Point startPoint, Point endPoint)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    this->create();
    this->setContentSize(Size(endPoint.x - startPoint.x,
                              endPoint.y - startPoint.y));
    
    this->startPoint = startPoint;
    this->endPoint = endPoint;
    
    cellSize = Size(0.044 * visibleSize.width,
                    0.0781 * visibleSize.height);
    
    size.width = round((endPoint.y - startPoint.y) / cellSize.height);
    size.height = round((endPoint.x - startPoint.x) / cellSize.width);
    
    CCLOG("Grid width:%f height:%f", size.width, size.height);
    
    auto correction = 0.0132 * visibleSize.width;
    
    auto width = int(size.width);
    auto height = int(size.height);
    
    cellSize = Size(0.044 * visibleSize.width,
                    0.0781 * visibleSize.height);
    
    cells = new FieldCell**[width];
    
    for(int i = 0; i < width; i++) {
        cells[i] = new FieldCell*[height];
        for(int j = 0; j < height; j++) {
            cells[i][j] = new FieldCell(Vec2(i,j),
                                        Point(correction + j * cellSize.width,
                                             startPoint.y + i * cellSize.height), cellSize);
            this->addChild(cells[i][j]);
        }
    }
    
    
    CCLOG("Start Point x:%f y:%f", startPoint.x, startPoint.y);
    CCLOG("End Point x:%f y:%f", endPoint.x, endPoint.y);
    CCLOG("Corrrection Point x:%f", correction);
}

rapidjson::Document Grid::createJSON()
{
    rapidjson::Document gridJSON;
    gridJSON.SetObject();
    rapidjson::Document::AllocatorType& allocator = gridJSON.GetAllocator();
    
    auto width = int(size.width);
    auto height = int(size.height);
    
    for(int i = 0; i < width; i++) {
        rapidjson::Value rowValue(rapidjson::kArrayType);
        for(int j = 0; j < height; j++) {
            rowValue.PushBack(cells[i][j]->getState(), allocator);
        }
        rapidjson::Value name(to_string(i).c_str(), allocator);
        gridJSON.AddMember(name, rowValue, allocator);
    }
    
    return gridJSON;
}

void Grid::exportGrid() {
    auto path = FileUtils::getInstance()->getWritablePath() + "grid.json";
    CCLOG("Write Path :%s", path.c_str());
    ofstream outputFile;
    outputFile.open(path);
    if(outputFile.is_open()) {
        string jsonObjectData = JsonToString(createJSON());
        outputFile << jsonObjectData;
    }
    else {
        log("File was not open");
    }
    outputFile.close();
}

std::string Grid::JsonToString(rapidjson::Document jsonGrid) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(buffer);
    jsonGrid.Accept(jsonWriter);
    return buffer.GetString();
}

rapidjson::Document Grid::readJSON()
{
    ifstream inputFile;
    rapidjson::Document gridJSON;
    gridJSON.SetObject();
    
    auto path = FileUtils::getInstance()->getWritablePath() + "grid.json";
    CCLOG("Read Path :%s", path.c_str());
    
    
    inputFile.open(path);
    if(inputFile.is_open()) {
        string jsonString;
        inputFile >> jsonString;
        //CCLOG("Was readed from file: %s", jsonString.c_str());
        gridJSON.Parse(jsonString.c_str());
    }
    else {
        log("Read File was not open");
    }
    inputFile.close();
    return gridJSON;
}

bool Grid::importGridFromJson()
{
    rapidjson::Document gridJSON;
    bool wasImported = false;
    
    gridJSON = readJSON();
    if(getJsonLength(gridJSON) > 0) {
        auto width = int(size.width);
        auto height = int(size.height);

        for(int i = 0; i < width; i++) {
            rapidjson::Value& rowValues = gridJSON[to_string(i).c_str()];
            assert(rowValues.IsArray());
            for(int j = 0; j < height; j++) {
                if(cells[i][j]->getState() != rowValues[j].GetInt()) {
                    cells[i][j]->changeState();
                }
            }
        }
        
        wasImported = true;
    }
    
    return wasImported;
}

void Grid::showGridState()
{
    auto width = int(size.width);
    auto height = int(size.height);
    
    log("");
    
    for(int i = width - 1; i >= 0; i--) {
        string rows = "";
        for(int j = 0; j < height; j++) {
            rows.append(to_string(cells[i][j]->getState())) + " ";
        }
        CCLOG("%s", rows.c_str());
    }
    log("");
}

int Grid::getJsonLength(rapidjson::Document &jsonGrid)
{
    rapidjson::Value::MemberIterator iterator;
    int length = 0;
    
    for (iterator = jsonGrid.MemberBegin(); iterator != jsonGrid.MemberEnd(); iterator++)
    {
        length++;
    }
    
    return length;
}