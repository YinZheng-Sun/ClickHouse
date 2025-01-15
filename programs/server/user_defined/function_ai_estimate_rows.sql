CREATE FUCTION imdb.ai_estimate_rows TYPE SCALAR RETURNS UInt64 LANGUAGE PYTHON AS 
$code$
from iudf import IUDF
from overload import overload
from BayesCard.Evaluation.cardinality_estimation import test_inference_result

class ai_estimate_rows(IUDF):
    @overload
    def process(sql):
        res = test_inference_result(sql)
        return res
$code$
