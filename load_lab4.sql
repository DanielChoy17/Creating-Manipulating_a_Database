--Script to populate the Highway Info schema

-- Highways(highwayNum, length, speedLimit)
COPY Highways FROM stdin USING DELIMITERS '|';
17|102.6|55
1|150.3|40
280|200.9|70
55|150.0|45
13|45.5|35
\.

-- Exits(highwayNum, exitNum, description, mileMarker, exitCity, exitState, isExitOpen)
COPY Exits FROM stdin USING DELIMITERS '|';
17|1|Emerald Cit|0.0|Oz|CA|TRUE
17|23|Hobbiton|98.6|The Shire|CA|FALSE
1|1|Emerald City|0.0|Oz|CA|TRUE
1|98|Omatikaya|150.3|Pandora|CA|FALSE
280|1|Hobbiton|0.0|Santa Cruz|CA|TRUE
280|25|Elronds Home|88.7|Rivendell|CA|FALSE
280|34|Mos Eisley|91.5|Tattooine|OR|TRUE
280|46|Jawa Camp|123.0|Tattooine|OR|FALSE
280|95|Jundland Wastes|124.3|Austin|TX|TRUE
17|34|Mos Eisley|4.5|Portland|OR|TRUE
1|95|Jundland Wastes|146.7|Tattooine|OR|FALSE
\.

-- Interchanges(highwayNum1, exitNum1, highwayNum2, exitNum2)
COPY Interchanges FROM stdin USING DELIMITERS '|';
1|1|17|1
280|34|280|25
17|23|280|1
17|34|280|46
17|34|280|34
1|95|280|95
\.

-- Cameras(cameraID, highwayNum, mileMarker, isCameraWorking)
COPY Cameras FROM stdin USING DELIMITERS '|';
901|1|0.0|TRUE
902|1|15.1|FALSE
903|1|150.3|TRUE
921|17|0.0|FALSE
927|17|98.6|FALSE
943|280|0.0|TRUE
945|280|88.7|TRUE
949|280|200.9|TRUE
951|55|50.5|FALSE
969|55|87.3|FALSE
856|13|10.0|FALSE
932|13|15.5|FALSE
960|13|25.0|FALSE
973|13|35.5|FALSE
\.


-- Owners(ownerState, ownerLicenseID, name, address, startDate, expirationDate)
COPY Owners FROM stdin USING DELIMITERS '|';
CA|N179244|\N|123 Main St|2018-01-09|2022-01-09|0|0
OR|N179244|Tom Johnson|333 Meder Dr|2011-03-21|\N|0|0
OR|P622557|Siobhan Roy|9931 El Camino|\N|2022-12-16|0|0
CA|T233186|Greg Hirsch|831 Santa Cruz Ave|2021-06-30|2024-03-15|0|0
CA|X482015|\N|123 Main St|2018-12-17|\N|0|0
OR|B385229|Gerri Kellman|222 Emmet Grade|2016-07-31|2023-06-02|0|0
WA|K569201|Stewart Rossellini|11 Private Dr|\N|\N|0|0
WA|B385226|Logan Schwartz|222 Emmet Grade|\N|2024-05-16|0|0
NJ|N179244|Gerri Kellman|101 W34 St|2020-11-11|2023-12-12|0|0
NC|B385226|Siobhan Roy|200 Melody Drive|2014-07-31|2022-04-12|0|0
NJ|P423554|Gerri Kellman|050 Silver Canoe Way|2017-01-31|\N|0|0
\.

-- Vehicles(vehicleState, vehicleLicensePlate, ownerState, ownerLicenseID, year, color) 
COPY Vehicles FROM stdin USING DELIMITERS '|';
CA|3XYZ123|CA|N179244|2020|RE
CA|7UVW177|OR|N179244|2019|GR
CA|3YHT835|OR|B385229|2014|BL
OR|3YHT835|OR|B385229|2014|BL
OR|3XYZ123|CA|T233186|2019|BK
CA|RELATIO|CA|X482015|2001|WH
CA|6KDD482|WA|B385226|2003|RE
OR|5PQR564|CA|X482015|2012|BK
OR|7UVW177|OR|B385229|2022|RE
NJ|3XYZ123|NJ|P423554|2023|BK
CA|1WES905|WA|B385226|2007|RE
CA|5PQR564|NJ|N179244|2023|RE
CA|5TMM623|WA|K569201|2006|RE
\.

-- Photos(cameraID, vehicleState, vehicleLicensePlate, photoTimestamp)
COPY Photos FROM stdin USING DELIMITERS '|';
901|CA|7UVW177|2022-12-01 16:20:01
902|CA|3YHT835|2022-12-01 16:02:35
902|CA|7UVW177|2022-12-01 16:05:01
903|CA|7UVW177|2022-12-01 15:00:30
927|CA|1WES905|2022-12-01 11:24:11
949|OR|5PQR564|2022-12-01 09:35:45
945|OR|5PQR564|2022-12-01 08:00:30
949|CA|RELATIO|2022-12-02 05:23:23
973|OR|3XYZ123|2022-12-02 07:00:30
856|NJ|3XYZ123|2022-12-02 10:42:21
856|CA|5TMM623|2021-08-21 15:19:00
951|CA|5TMM623|2023-01-02 13:46:33
901|CA|3XYZ123|2022-12-04 09:20:01
903|CA|3XYZ123|2022-12-04 08:00:30
856|CA|5PQR564|2022-12-07 08:20:17
932|CA|5PQR564|2022-12-07 08:43:54
973|CA|5PQR564|2022-12-07 09:02:01
951|CA|RELATIO|2022-12-15 17:23:06
969|CA|RELATIO|2022-12-15 17:42:38
\.
