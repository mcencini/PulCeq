% classdef testPulseqShapes < matlab.unittest.TestCase
%     % Unit tests for pulseg.PulseqShapeArbitrary and pulseg.PulseqShapeTrap classes
%     
%     methods (Test)
%         function testPulseqShapeArbitrary_serialization(testCase)
%             % Test serialization and deserialization of pulseg.PulseqShapeArbitrary
%             
%             % Create sample data
%             samples = single([0.1, 0.2, 0.3, 0.4, 0.5]);
%             shape = pulseg.PulseqShapeArbitrary(samples);
%             
%             % Serialize (default little-endian)
%             serializedData = shape.to_bytes(false);
%             
%             % Deserialize
%             deserializedShape = pulseg.PulseqShapeArbitrary.from_bytes(serializedData, false);
%             
%             % Verify that the deserialized samples match original samples
%             testCase.verifyEqual(deserializedShape.samples, samples, "AbsTol", 1e-6);
%         end
%         
%         function testPulseqShapeArbitrary_serialization_bigEndian(testCase)
%             % Test serialization and deserialization of pulseg.PulseqShapeArbitrary with big-endian
%             
%             samples = single([0.1, 0.2, 0.3, 0.4, 0.5]);
%             shape = pulseg.PulseqShapeArbitrary(samples);
%             
%             % Serialize (big-endian)
%             serializedData = shape.to_bytes(true);
%             
%             % Deserialize (big-endian)
%             deserializedShape = pulseg.PulseqShapeArbitrary.from_bytes(serializedData, true);
%             
%             % Verify that the deserialized samples match original samples
%             testCase.verifyEqual(deserializedShape.samples, samples, "AbsTol", 1e-6);
%         end
%         
%         function testPulseqShapeTrap_serialization(testCase)
%             % Test serialization and deserialization of pulseg.PulseqShapeTrap
%             
%             riseTime = int32(10);
%             flatTime = int32(20);
%             fallTime = int32(30);
%             shape = pulseg.PulseqShapeTrap(riseTime, flatTime, fallTime);
%             
%             % Serialize (default little-endian)
%             serializedData = shape.to_bytes(false);
%             
%             % Deserialize
%             deserializedShape = pulseg.PulseqShapeTrap.from_bytes(serializedData, false);
%             
%             % Verify that deserialized values match original values
%             testCase.verifyEqual(deserializedShape.riseTime, riseTime);
%             testCase.verifyEqual(deserializedShape.flatTime, flatTime);
%             testCase.verifyEqual(deserializedShape.fallTime, fallTime);
%         end
%         
%         function testPulseqShapeTrap_serialization_bigEndian(testCase)
%             % Test serialization and deserialization of pulseg.PulseqShapeTrap with big-endian
%             
%             riseTime = int32(10);
%             flatTime = int32(20);
%             fallTime = int32(30);
%             shape = pulseg.PulseqShapeTrap(riseTime, flatTime, fallTime);
%             
%             % Serialize (big-endian)
%             serializedData = shape.to_bytes(true);
%             
%             % Deserialize (big-endian)
%             deserializedShape = pulseg.PulseqShapeTrap.from_bytes(serializedData, true);
%             
%             % Verify that deserialized values match original values
%             testCase.verifyEqual(deserializedShape.riseTime, riseTime);
%             testCase.verifyEqual(deserializedShape.flatTime, flatTime);
%             testCase.verifyEqual(deserializedShape.fallTime, fallTime);
%         end
%     end
% end
