typedef std::pair<std::string,std::vector<double> > tick_data; //pair<datetime string, vector of price data>
typedef std::map<std::string,std::vector<tick_data> > market; //map: symbol --> vector of tick data for that symbol. This vector will only have max_ticks elements
typedef std::vector<std::tuple<std::string,int,double> > orderlist;