--Image avec peu de vert et beaucoup de rouge --

SELECT name, iscolored, rater, rateg, rateb, meanGradient, pixelContour FROM MULTIMEDIA
WHERE iscolored = 1 AND rateg < 0.5 AND rater > 0.5
ORDER BY (rater) DESC

--Image en noir et blanc --

SELECT name, iscolored, rater, rateg, rateb, meanGradient, pixelContour FROM multimedia
WHERE iscolored = 0

--Image avec le taux de Texture le plus elevé--
SELECT name, iscolored, rater, rateg, rateb, meanGradient, pixelContour FROM multimedia
WHERE meangradient > (SELECT AVG(meanGradient) FROM multimedia) AND pixelContour > (SELECT AVG(pixelContour) FROM multimedia)
ORDER BY meangradient DESC

--Comparaison --

SELECT * FROM
    (
        SELECT img1_name, img2_name, distance_similarity FROM dist_eucl_img_hist
        WHERE img1_name = '406.jpg' AND img2_name != img1_name
        ORDER BY(distance_similarity) ASC
    ) resultSet
WHERE ROWNUM <= 1;

SELECT * FROM
    (
        SELECT img1_name, img2_name, distance_similarity FROM dist_bhatt_img_hist
        WHERE img1_name = '406.jpg' AND img2_name != img1_name
        ORDER BY(distance_similarity) ASC
    ) resultSet
WHERE ROWNUM <= 1;


SELECT * FROM
    (
        SELECT img1_name, img2_name, distance_similarity FROM dist_signature_img
        WHERE img1_name = '406.jpg' AND img2_name != img1_name
        ORDER BY(distance_similarity) ASC
    ) resultSet
WHERE ROWNUM <= 1;

