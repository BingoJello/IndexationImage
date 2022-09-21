--Image avec peu de vert et beaucoup de rouge --

SELECT name, iscolored, rater, rateg, rateb, meanGradient, pixelContour FROM MULTIMEDIA
WHERE iscolored = 1 AND rateg < 0.5 AND rater > 0.5
ORDER BY (rater) DESC

--Image en noir et blanc --

SELECT name, iscolored, rater, rateg, rateb, meanGradient, pixelContour FROM multimedia
WHERE iscolored = 1

--Image avec le taux de Texture le plus elevé--
SELECT name, iscolored, rater, rateg, rateb, meanGradient, pixelContour FROM multimedia
WHERE meangradient > (SELECT AVG(meanGradient) FROM multimedia) AND pixelContour > (SELECT AVG(pixelContour) FROM multimedia)
ORDER BY meangradient DESC

--Comparaison --

SELECT img1_name, img2_name, distance_similarity FROM dist_eucl_img_hist
WHERE img1_name = '23.jpg' AND distance_similarity < 2
ORDER BY(distance_similarity) ASC

SELECT img1_name, img2_name, distance_similarity FROM dist_bhatt_img_hist
WHERE img1_name = '23.jpg' AND distance_similarity < 2
ORDER BY(distance_similarity) ASC

SELECT img1_name, img2_name, distance_similarity FROM dist_signature_img
WHERE img1_name = '23.jpg' AND distance_similarity < 2
ORDER BY(distance_similarity) ASC
