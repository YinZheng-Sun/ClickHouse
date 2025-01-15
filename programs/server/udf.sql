DROP FUNCTION IF EXISTS ai_estimate_rows;

CREATE FUNCTION imdb.ai_estimate_rows
RETURNS UInt64
LANGUAGE PYTHON AS
$code$
from iudf import IUDF
from overload import overload
from BayesCard.Evaluation.cardinality_estimation import test_inference_result

class ai_estimate_rows(IUDF):
    @overload
    def process(sql):
        res = test_inference_result(sql)
        return res
$code$;

DROP FUNCTION IF EXISTS ai_estimate_rows;

select imdb.ai_estimate_rows('SELECT COUNT(*) FROM movie_info_idx WHERE id >= 713168 AND movie_id >= 2075341');
