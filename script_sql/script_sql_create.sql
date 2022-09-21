-- noinspection SqlNoDataSourceInspectionForFile

set serveroutput on;

CREATE TABLE multimedia(
    name varchar2(50) primary key NOT NULL,
    isColored NUMBER(1,0),
    rateR float,
    rateG float,
    rateB float,
    pixelContour float,
    meanGradient float,
    histogramme CLOB,
    image ordsys.ordimage,
    signature ordsys.ordimageSignature
);


CREATE TABLE dist_bhatt_img_hist(
    img1_name VARCHAR2(10),
    img2_name VARCHAR2(10),
    distance_similarity float
);

CREATE TABLE dist_eucl_img_hist(
    img1_name VARCHAR2(10),
    img2_name VARCHAR2(10),
    distance_similarity float
);

CREATE TABLE dist_signature_img(
    img1_name VARCHAR2(10),
    img2_name VARCHAR2(10),
    distance_similarity float
);

CREATE TABLE hist_img (
    name_img VARCHAR2(10),
    index_hist int,
    value_hist float
);