#include <iostream>

#include <vector>

#include <map>

#include <fstream>

#include <sstream>

#include <string>

#include <climits>

#include <cmath>

#include <cstdlib>

#include <ctime>


class Point

{

public:

    void normalize()

    {

        for(unsigned int i=0;i<m_coordinates.size();++i)

        {

            if(m_coordinates[i]==-300)

                m_coordinates[i] = 0;

            else

                m_coordinates[i] += 100;

        }

    }


    std::vector<double> m_coordinates;

};




class Probabilities

{

public:

    Probabilities()

    {

        for(unsigned int i=0;i<100;++i)

        {

            probabilities[i] = 0.0;

        }

    }


    void addMeasurement(unsigned int p_value)

    {

        probabilities[p_value] = 1.0;

        double val = 0.5;

        for(unsigned int i=1;i<100;++i)

        {

            if(p_value>=i && probabilities[p_value-i]<val)

                probabilities[p_value-i] = val;

            if(p_value+i<100 && probabilities[p_value+i]<val)

                probabilities[p_value+i] = val;

            val/=2;

        }

    }


    double getProbability(unsigned int p_value) const

    {

        return probabilities[p_value];

    }


private:

    double probabilities[100];

};


class RoomProbability

{

public:

    void addMeasurement(unsigned int p_ap, unsigned int p_value)

    {

        apProbabilities[p_ap].addMeasurement(p_value);

    }


    double getProbability(unsigned int p_ap, unsigned int p_value) const

    {

        auto it = apProbabilities.find(p_ap);

        if(it==apProbabilities.end())

            return 0.01;

        return it->second.getProbability(p_value);

    }


private:

    std::map<unsigned int, Probabilities> apProbabilities;

};


class RoomData

{

public:


    void insertData(const Point& point)

    {

        for(unsigned int i=0;i<point.m_coordinates.size();++i)

        {

            if(point.m_coordinates[i]>0)

            {

                roomProbability.addMeasurement(i, point.m_coordinates[i]);

            }

        }

    }


    double getProbability(const Point& point) const

    {

        double result = 1.0;

        for(unsigned int i=0;i<point.m_coordinates.size();++i)

        {

            if(point.m_coordinates[i]>0)

            {

                result *= roomProbability.getProbability(i, point.m_coordinates[i]);

            }

        }

        return result;

    }


private:

    RoomProbability roomProbability;

};


class Data

{

public:

    Data(std::ifstream& file)

    {

        char dataBuf[5000];

        while(file.getline(dataBuf, 5000))

        {

            std::pair<std::string,Point> info = getData(dataBuf);

            if(rand()%100 > 10)

            {

                data[info.first].insertData(info.second);

            }

            else

            {

               m_rawData.push_back(info);;

            }

        }

    }


    std::string checkMeasurement(const Point& point) const

    {

        double max = 0.0;

        std::string room;

        for(auto it = data.begin(); it!= data.end();++it)

        {

            double res = it->second.getProbability(point);

            if(res>max)

            {

                max = res;

                room = it->first;

            }

        }

        return room;

    }



    void verify() const

    {

        int totalCount = 0;

        int mistakes = 0;

        for(auto it = m_rawData.begin(); it!= m_rawData.end();++it)

        {

            std::string res = this->checkMeasurement(it->second);

            totalCount++;

            if(it->first != res)

                mistakes++;

        }

        std::cout << totalCount << ", " << mistakes << std::endl;

    }


private:

    std::pair<std::string,Point> getData(char* charData)

    {

        std::stringstream ss(charData);

        std::string roomName;


        std::getline(ss, roomName, ';');


        Point meas;

        std::string segment;

        while(std::getline(ss, segment, ';'))

        {

           meas.m_coordinates.push_back(std::stoi(segment));

        }

        meas.normalize();

        return {roomName,meas};


    }


    std::map<std::string, RoomData> data;

    std::vector< std::pair<std::string, Point> > m_rawData;

};


int main(int argc, char * argv[])

{

    if(argc != 2)

    {

        std::cout<<"usage: "<<argv[0]<<" filename"<<std::endl;

        return -1;

    }

    std::srand(std::time(nullptr));

    for(int i = 0;i <100;++i)

    {

        std::ifstream file(argv[1]);

        if(!file.is_open())

        {

            std::cout<<"file not exist"<<std::endl;

            return -1;

        }

        Data data(file);

        file.close();

        data.verify();

    }

    return 0;

}
